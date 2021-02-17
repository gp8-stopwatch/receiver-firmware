/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Detector.h"
#include "Config.h"

#ifndef UNIT_TEST
#include "Gpio.h"
Gpio senseOn{GPIOB, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
#include "Container.h"
#else
#define __disable_irq(x) x
#define __enable_irq(x) x
auto &getConfig ()
{
        static cfg::Config config;
        return config;
}
#endif

/*****************************************************************************/

void EdgeFilter::onEdge (Edge const &e)
{
        /*
         * This can happen when noise frequency is very high, and the µC can't keep up,
         * and its misses an EXTI event. This way we can end up with two consecutive edges
         * with the same polarity.
         * When compiled with -O0 the fastest it can keep up with is :
         * - 3.33kHz with 1/3 duty (300µs period 100µs spikes)
         * - 7.5kHz (150µs period) with duty changing between ~30-70%. Rate of this change is 200Hz. This pattern is stored on my s.gen.
         * When compiled with -O3 the fastest is 10kHz with 50% duty cycle, so IRQ called every 50µs
         */
        if (!queue.empty () && queue.back ().polarity == e.polarity) {
                // Reset queue so it's still full, but pulses are 0 width. This will automatically increase noiseCounter by 2
                queue.push (e);
                queue.push ({e.timePoint, (e.polarity == EdgePolarity::rising) ? (EdgePolarity::falling) : (EdgePolarity::rising)});

                /*
                 * TODO test and decide what to do here. To some extent the device can recover.
                 * Although the screen starts to flicker un such cases.
                 */

                // while (true) {
                // }
        }

        queue.push (e);

        /*--------------------------------------------------------------------------*/

        // Calculate duty cycle of present slice of the signal
        Result1us cycleTreshold = (queue.back ().timePoint - queue.front ().timePoint)
                * getConfig ().getDutyTresholdPercent ();               // slice length times treshold. See equation in the docs.
        Result1us hiDuration = queue[1].timePoint - queue[0].timePoint; // We assume for now, that queue[0] is rising
        Result1us lowDuration = queue[2].timePoint - queue[1].timePoint;

        /*--------------------------------------------------------------------------*/

        // Find out which edge in the queue is rising, which falling.
        auto *firstRising = &queue[0]; // Pointers are smaller than Edge object
        auto *firstFalling = &queue[1];

        bool longHighEdge{};
        bool longLowEdge{};

        // 50% chance that above are correct. If our assumption wasn't right, we swap.
        if (firstRising->polarity != EdgePolarity::rising) {
                std::swap (firstRising, firstFalling);
                std::swap (hiDuration, lowDuration);
                // Take only recent level. If last level was high, check if its duration was long enough.
                longHighEdge = hiDuration >= minTriggerEvent1Us;

                if (!longHighEdge) { // False means that a level was shorter than the trigger event.
                        ++noiseCounter;
                }
        }
        else {
                // Take only recent level. If last level was low, check if its duration was long enough.
                longLowEdge = lowDuration >= minTriggerEvent1Us;

                if (!longLowEdge) { // False means that a level was shorter than the trigger event.
                        ++noiseCounter;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* State transitions depending on dutyCycle and recent level length.        */
        /*--------------------------------------------------------------------------*/

        if (hiDuration * 100 > cycleTreshold || // PWM of the slice is high
            longHighEdge) {                     // Or the high level was long itself
                if (state != State::high) {
                        state = State::high;
#ifndef UNIT_TEST
                        senseOn.set (true);
#endif
                        highStateStart = firstRising->timePoint;
                }
        }
        else if (lowDuration * 100 >= cycleTreshold || // PWM of the slice is low
                 longLowEdge) {                        // Or the low level was long enogh itself
                if (state != State::low) {
                        state = State::low;
#ifndef UNIT_TEST
                        senseOn.set (false);
#endif
                        lowStateStart = firstFalling->timePoint;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Check trigger event conditions.                                          */
        /*--------------------------------------------------------------------------*/

        if (highStateStart < lowStateStart) {
                bool longHighState = (lowStateStart - highStateStart) >= minTriggerEvent1Us;
                bool longLowState = (e.timePoint - lowStateStart) >= minTriggerEvent1Us;

                if (longHighState && longLowState) {
                        callback->report (DetectorEventType::trigger, highStateStart);
                        highStateStart = lowStateStart; // To prevent reporting twice
                }
        }
}

/****************************************************************************/

void EdgeFilter::run (Result1us const &now)
{
        __disable_irq ();
        if (queue.empty ()) {
                return;
        }

        auto back = queue.back ();
        auto currentState = state;
        auto currentHighStateStart = highStateStart;
        auto currentLowStateStart = lowStateStart;
        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Steady state prevention                                                  */
        /*--------------------------------------------------------------------------*/

        // If there's no noise at all, and the line stays silent, we force the check every minTreggerEventMs
        if (now - back.timePoint >= minTriggerEvent1Us && back.polarity == EdgePolarity::falling) {

                bool longHighState{};
                bool longLowState{};

                /*
                 * This is case when we aer still in high state, because no PWM was calculated.
                 * My previous impl. was inserting fake noise spikes to recalcutale the PWM and
                 * thus state.
                 */
                if (currentState == State::high && back.timePoint > currentHighStateStart) {
                        longHighState = (back.timePoint - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = (now - back.timePoint) >= minTriggerEvent1Us;
                }

                /*
                 * And this condition is the same as in onEvent's case
                 */
                else if (currentState == State::low && currentHighStateStart < currentLowStateStart) {
                        longHighState = (currentLowStateStart - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = (now - currentLowStateStart) >= minTriggerEvent1Us;
                }

                if (longHighState && longLowState) {
                        callback->report (DetectorEventType::trigger, currentHighStateStart);
                        __disable_irq ();
                        highStateStart = currentLowStateStart; // To prevent reporting twice
                        __enable_irq ();
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Noise detection + hysteresis                                             */
        /*--------------------------------------------------------------------------*/
        if (now - lastNoiseCalculation >= msToResult1us (NOISE_CALCULATION_PERIOD_MS)) {
                __disable_irq ();
                auto currentNoiseCounter = noiseCounter;
                noiseCounter = 0;
                __enable_irq ();

                if (currentNoiseCounter > 0) {
                        // This is typical case, where there is some noise present, and we normalize it from 1 to 15.
                        // TODO this will have to be adjusted in direct sunlight in July or August.
                        // TODO or find the 100W lightbulb I've lost. If it manages to achieve level 15, then it's OK.
                        noiseLevel = std::min<uint8_t> ((MAX_NOISE_LEVEL - 1) * currentNoiseCounter * MIN_NOISE_SPIKE_1US
                                                                / msToResult1us (NOISE_CALCULATION_PERIOD_MS),
                                                        (MAX_NOISE_LEVEL - 1))
                                + 1;
                }
                else {
                        // 0 is a special case where there is absolutely no noise. Levels 1-15 are proportional.
                        noiseLevel = 0;
                }

                if (noiseState == NoiseState::noNoise && noiseLevel >= getConfig ().getNoiseLevelHigh ()) {
                        noiseState = NoiseState::noise;
                        // TODO When noise counter is high, turn of the EXTI, so the rest of the code has a chance to run. Then enable it
                        // after 1s
                        callback->report (DetectorEventType::noise, now);
                }
                else if (noiseState == NoiseState::noise && noiseLevel <= getConfig ().getNoiseLevelLow ()) {
                        noiseState = NoiseState::noNoise;
                        callback->report (DetectorEventType::noNoise, now);
                }

                lastNoiseCalculation = now;
        }

        /*--------------------------------------------------------------------------*/
        /* No beam detection + hysteresis                                           */
        /*--------------------------------------------------------------------------*/
        // if (now - lastBeamStateCalculation >= msToResult1 (minTreggerEventMs * 10)) {

        //         if (beamState == BeamState::present &&) {
        //                 beamState = BeamState::absent;
        //                 callback->report (DetectorEventType::noBeam, now);
        //         }
        //         else if (beamState == BeamState::absent &&) {
        //                 beamState = BeamState::present;
        //                 callback->report (DetectorEventType::beamRestored, now);
        //         }

        //         lastBeamStateCalculation = now;
        //         noiseCounter = 0;
        // }
}

/****************************************************************************/

void EdgeFilter::recalculateConstants ()
{
        __disable_irq ();
        minTriggerEvent1Us = msToResult1us (getConfig ().getMinTreggerEventMs ());
        __enable_irq ();
}
