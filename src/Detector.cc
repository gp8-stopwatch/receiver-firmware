/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Detector.h"

#ifndef UNIT_TEST
#include "Gpio.h"
Gpio senseOn{GPIOB, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
#include "Container.h"
#else
#define __disable_irq(x) x
#define __enable_irq(x) x
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
        Result1us cycleTresholdCalculated = (queue.back ().timePoint - queue.front ().timePoint)
                * getConfig ().getDutyTresholdPercent ();               // slice length times treshold. See equation in the docs.
        Result1us hiDuration = queue[1].timePoint - queue[0].timePoint; // We assume for now, that queue[0] is rising
        Result1us lowDuration = queue[2].timePoint - queue[1].timePoint;

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

        highStateAveragePeriod += hiDuration;

        /*--------------------------------------------------------------------------*/
        /* PWM State transitions depending on dutyCycle and recent level length.    */
        /*--------------------------------------------------------------------------*/

        if (hiDuration * 100 > cycleTresholdCalculated || // PWM of the slice is high
            longHighEdge)                                 // Or the high level was long itself
        {
                if (pwmState != PwmState::high) {
                        pwmState = PwmState::high;
#ifndef UNIT_TEST
                        senseOn.set (true);
#endif
                        highStateStart = firstRising->timePoint;
                }
        }
        else if (lowDuration * 100 >= cycleTresholdCalculated || // PWM of the slice is low
                 longLowEdge) {                                  // Or the low level was long enogh itself
                if (pwmState != PwmState::low) {
                        pwmState = PwmState::low;
#ifndef UNIT_TEST
                        senseOn.set (false);
#endif
                        lowStateStart = firstFalling->timePoint;
                }
        }
        else if (pwmState != PwmState::middle) { // Previous conditions for level durations weren't satisfied
                pwmState = PwmState::middle;
                middleStateStart = firstFalling->timePoint;
        }

        /*--------------------------------------------------------------------------*/
        /* Trigger level state transitions. Kind of an intermediate step.           */
        /*--------------------------------------------------------------------------*/

        /*
                if (pwmState == PwmState::high && e.timePoint - highStateStart >= minTriggerEvent1Us && // Duty cycle is high for at least
           minTriggerMs triggerLevelState == TriggerLevelState::idle) {                                     // triggerState is idle
                        triggerLevelState = TriggerLevelState::high;
                }
                else if (pwmState == PwmState::low && e.timePoint - lowStateStart >= minTriggerEvent1Us && // Duty is low for at least
           minTriggerMs triggerLevelState == TriggerLevelState::high) {                                   // triggerState is correct
                        // triggerLevelState = TriggerLevelState::low;
                        callback->report (DetectorEventType::trigger, highStateStart);
                        triggerLevelState = TriggerLevelState::idle;
                }
        */

        /*--------------------------------------------------------------------------*/
        /* Check trigger event conditions.                                          */
        /*--------------------------------------------------------------------------*/

        if (highStateStart < lowStateStart &&    // Correct order of states : first middleState, then High, and at the end low
            middleStateStart < highStateStart) { // No middle state between high and low
                bool longHighState = (lowStateStart - highStateStart) >= minTriggerEvent1Us;
                bool longLowState = (e.timePoint - lowStateStart) >= minTriggerEvent1Us;

                if (longHighState && longLowState) {
                        callback->report (DetectorEventType::trigger, highStateStart);
                        highStateStart = middleStateStart = lowStateStart; // To prevent reporting twice
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
        auto currentState = pwmState;
        auto currentHighStateStart = highStateStart;
        auto currentLowStateStart = lowStateStart;
        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Steady state prevention                                                  */
        /*--------------------------------------------------------------------------*/

        auto tp = std::min (back.timePoint, currentLowStateStart);

        // If there's no noise at all, and the line stays silent, we force the check every minTreggerEventMs
        if (now - tp >= minTriggerEvent1Us && back.polarity == EdgePolarity::falling /* && triggerLevelState == TriggerLevelState::high */) {

                bool longHighState{};
                bool longLowState{};

                /*
                 * This is case when we are still in high state, because no PWM was calculated.
                 * My previous impl. was inserting fake noise spikes to recalcutale the PWM and
                 * thus state. This was inefficient.
                 */
                if (currentState == PwmState::high && back.timePoint > currentHighStateStart) {
                        longHighState = (back.timePoint - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = (now - back.timePoint) >= minTriggerEvent1Us;
                }
                /*
                 * And this condition is the same as in onEvent's case
                 */
                else if (currentState == PwmState::low && currentHighStateStart < currentLowStateStart) {
                        longHighState = (currentLowStateStart - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = (now - currentLowStateStart) >= minTriggerEvent1Us;
                }

                if (longHighState && longLowState) {
                        callback->report (DetectorEventType::trigger, currentHighStateStart);
                        // triggerLevelState = TriggerLevelState::idle;
                        __disable_irq ();
                        highStateStart = middleStateStart = lowStateStart; // To prevent reporting twice
                        __enable_irq ();
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Noise detection + hysteresis                                             */
        /*--------------------------------------------------------------------------*/
        if (now - lastNoiseCalculation >= msToResult1us (NOISE_CALCULATION_PERIOD_MS)) {
                lastNoiseCalculation = now;

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

                bool stateChanged{};

                if (noiseState == NoiseState::noNoise && noiseLevel >= getConfig ().getNoiseLevelHigh ()) {
                        noiseState = NoiseState::noise;
                        stateChanged = true;
                        // TODO When noise counter is high, turn of the EXTI, so the rest of the code has a chance to run. Then enable it
                        // after 1s
                        callback->report (DetectorEventType::noise, now);
                }
                else if (noiseState == NoiseState::noise && noiseLevel <= getConfig ().getNoiseLevelLow ()) {
                        noiseState = NoiseState::noNoise;
                        stateChanged = true;
                        callback->report (DetectorEventType::noNoise, now);
                }

                // if (stateChanged) { // No point of calculating trigger if there's no beam, or it was just restored.
                //         __disable_irq ();
                //         highStateStart = middleStateStart = lowStateStart;
                //         __enable_irq ();
                //         return;
                // }
        }

        /*--------------------------------------------------------------------------*/
        /* No beam detection + hysteresis                                           */
        /*--------------------------------------------------------------------------*/
        // TODO what if trigger ms is bigger than msToResult1us (NO_BEAM_CALCULATION_PERIOD_MS)
        if (now - lastBeamStateCalculation >= msToResult1us (NO_BEAM_CALCULATION_PERIOD_MS)) {
                lastBeamStateCalculation = now;

                __disable_irq ();
                auto currentHighStateAveragePeriod = highStateAveragePeriod;
                highStateAveragePeriod = 0;
                __enable_irq ();

                uint8_t currentHighStateAveragePeriodPerccent
                        = 100 * currentHighStateAveragePeriod / msToResult1us (NO_BEAM_CALCULATION_PERIOD_MS);

                bool stateChanged{};

                if (beamState == BeamState::present && currentHighStateAveragePeriodPerccent >= getConfig ().getDutyTresholdPercent ()) {
                        beamState = BeamState::absent;
                        stateChanged = true;
                        callback->report (DetectorEventType::noBeam, now);
                }
                else if (beamState == BeamState::absent && currentHighStateAveragePeriodPerccent < getConfig ().getDutyTresholdPercent ()) {
                        beamState = BeamState::present;
                        stateChanged = true;
                        callback->report (DetectorEventType::beamRestored, now);
                }

                // if (stateChanged) { // No point of calculating trigger if there's no beam, or it was just restored.
                //         __disable_irq ();
                //         highStateStart = middleStateStart = lowStateStart;
                //         __enable_irq ();
                //         return;
                // }
        }
}

/****************************************************************************/

void EdgeFilter::recalculateConstants ()
{
        __disable_irq ();
        minTriggerEvent1Us = msToResult1us (getConfig ().getMinTreggerEventMs ());
        __enable_irq ();
}
