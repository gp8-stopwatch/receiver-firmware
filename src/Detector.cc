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
#define debugPin(x) senseOn.set (x)
#include "Container.h"
#else
#define __disable_irq(x) x
#define __enable_irq(x) x
#define debugPin(x)
#endif

/*****************************************************************************/

void EdgeFilter::onEdge (Edge const &e)
{
        // TODO turn off this EXTI and remove this condition (opt). EDIT : button would not work. The button pin should be changed then.
        if (!active || blindState == BlindState::blind) {
                return;
        }

        /*
         * This can happen when noise frequency is very high, and the µC can't keep up,
         * and its misses an EXTI event. This way we can end up with two consecutive edges
         * with the same polarity.
         * When compiled with -O0 the fastest it can keep up with is :
         * - 3.33kHz with 1/3 duty (300µs period 100µs spikes)
         * - 6.66kHz (150µs period) with duty changing between ~30-70%. Rate of this change is 200Hz. This pattern is stored on my s.gen.
         * When compiled with -O3 the fastest is 10kHz with 50% duty cycle, so IRQ called every 50µs
         * - 7.7 / 40%
         *
         * Release:
         * 14.3kHz / 40% / 200Hz (42µs + 28µs)
         * 16.6kHz
         *
         * TODO Pokazuje no i.r. (często/co drugi raz).
         * TODO Zaimplementować blind time przed kolejnymi testami.
         * TODO EDIT : chyba jednak nie jest to błąd. Przy dużych zakłóceniach widoczne błędne działanie algorytmu. Screeny w katalogu doc. z
         * dnia 24/02/2021
         * TODO Testy powinny symulowac dwór. Czysty sygnał zakłócamy żarówką 100W. Testy z odbiciami są prostsze (nie trzeba żarówki) ale to nie
         * ten use-case.
         */
        if (!queue.empty () && queue.back ().polarity == e.polarity) {
                // TODO there should be some bit in some register that would tell me that I've missed this ISR. This would be safer and cleaner
                // to use. Reset queue so it's still full, but pulses are 0 width. This will automatically increase noiseCounter by 2
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
                * getConfig ().getDutyTresholdPercent ();                           // slice length times treshold. See equation in the docs.
        Result1us hiDuration = queue.getE1 ().timePoint - queue.getE0 ().timePoint; // We assume for now, that queue.getE0() is rising
        Result1us lowDuration = queue.getE2 ().timePoint - queue.getE1 ().timePoint;

        // Find out which edge in the queue is rising, which falling.
        auto *firstRising = &queue.getE0 (); // Pointers are smaller than Edge object
        auto *firstFalling = &queue.getE1 ();

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
        /* PWM State transitions depending on dutyCycle and recent level length.    */
        /*--------------------------------------------------------------------------*/

        if (hiDuration * 100 > cycleTresholdCalculated || // PWM of the slice is high
            longHighEdge)                                 // Or the high level was long itself
        {
                if (pwmState != PwmState::high) {
                        pwmState = PwmState::high;
                        highStateStart = firstRising->timePoint;
                        debugPin (true);
                }
        }
        else if (lowDuration * 100 >= cycleTresholdCalculated || // PWM of the slice is low
                 longLowEdge) {                                  // Or the low level was long enogh itself
                if (pwmState != PwmState::low) {
                        pwmState = PwmState::low;
                        lowStateStart = firstFalling->timePoint;
                        debugPin (false);
                }
        }
        else if (pwmState != PwmState::middle) { // Previous conditions for level durations weren't satisfied
                pwmState = PwmState::middle;
                middleStateStart = firstFalling->timePoint;
                debugPin (false);
        }

        /*--------------------------------------------------------------------------*/
        /* Check trigger event conditions.                                          */
        /*--------------------------------------------------------------------------*/

        if (highStateStart < lowStateStart &&    // Correct order of states : first middleState, then High, and at the end low
            middleStateStart < highStateStart) { // No middle state between high and low
                bool longHighState = (lowStateStart - highStateStart) >= minTriggerEvent1Us;
                bool longLowState = (e.timePoint - lowStateStart) >= minTriggerEvent1Us;

                if (longHighState && longLowState && isBeamClean ()) {
                        callback->report (DetectorEventType::trigger, highStateStart);
                        blindState = BlindState::blind;
                        blindStateStart = e.timePoint;
                        reset (); // To prevent reporting twice
                }
        }
}

/****************************************************************************/

// TODO Run every half min trigger len.
void EdgeFilter::run (Result1us const &now)
{
        if (!active || queue.empty ()) {
                return;
        }

        /*--------------------------------------------------------------------------*/
        /* Blind state.                                                             */
        /*--------------------------------------------------------------------------*/
        if (blindState == BlindState::blind) {

                if (now - blindStateStart >= msToResult1us (getConfig ().getBlindTime ())) {
                        blindState = BlindState::notBlind;
                }
                else {
                        return;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Steady state detection, pwmState correction.                             */
        /*--------------------------------------------------------------------------*/
        __disable_irq ();
        auto back = queue.back ();
        __enable_irq ();

        Result1us lastSignalChange = back.timePoint;

        if (now - lastSignalChange >= minTriggerEvent1Us) {
                if (back.polarity == EdgePolarity::rising) {
                        if (pwmState != PwmState::high) {
                                __disable_irq ();
                                pwmState = PwmState::high;
                                highStateStart = lastSignalChange;
                                __enable_irq ();
                                debugPin (true);
                        }
                }
                else {
                        if (pwmState != PwmState::low) {
                                __disable_irq ();
                                pwmState = PwmState::low;
                                lowStateStart = lastSignalChange;
                                __enable_irq ();
                                debugPin (false);
                        }
                }
        }

        __disable_irq ();
        auto currentState = pwmState;
        auto currentHighStateStart = highStateStart;
        auto currentLowStateStart = lowStateStart;
        auto currentMiddleStateStart = middleStateStart;
        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Noise detection + hysteresis                                             */
        /*--------------------------------------------------------------------------*/
        if (now - lastNoiseCalculation >= msToResult1us (NOISE_CALCULATION_PERIOD_MS) && beamState != BeamState::absent) {
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

                if (noiseState == NoiseState::noNoise && noiseLevel >= getConfig ().getNoiseLevelHigh ()) {
                        noiseState = NoiseState::noise;
                        // TODO When noise counter is high, turn of the EXTI, so the rest of the code has a chance to run. Then enable it
                        // after 1s TODO button will stop working during this period.
                        __disable_irq ();
                        callback->report (DetectorEventType::noise, now);
                        reset ();
                        return;
                        __enable_irq ();
                }

                /* else */ if (noiseState == NoiseState::noise && noiseLevel <= getConfig ().getNoiseLevelLow ()) {
                        noiseState = NoiseState::noNoise;
                        __disable_irq ();
                        callback->report (DetectorEventType::noNoise, now);
                        reset ();
                        return;
                        __enable_irq ();
                }
        }

        /*--------------------------------------------------------------------------*/
        /* No beam detection + hysteresis                                           */
        /*--------------------------------------------------------------------------*/

        auto noBeamTimeoutMs = std::max (NO_BEAM_CALCULATION_PERIOD_MS, getConfig ().getMinTreggerEventMs ());

        if (now - lastBeamStateCalculation >= msToResult1us (noBeamTimeoutMs) && noiseState != NoiseState::noise) {
                // This is the real period during we gathered the data uised to determine if there is noBeam situation.
                auto actualNoBeamCalculationPeriod = now - lastBeamStateCalculation;
                lastBeamStateCalculation = now;

                if (beamState == BeamState::present && // State correct - beam was present before.
                    currentState == PwmState::high &&  // Pwm state is high (still is) for very long
                    /*
                     * For so long, that high time to noBeam calculation time (def 1s) is higher
                     * (or eq) than dutyCycle (50 by def.)
                     */
                    100 * (now - currentHighStateStart) >= getConfig ().getDutyTresholdPercent () * actualNoBeamCalculationPeriod) {

                        beamState = BeamState::absent;
                        __disable_irq ();
                        callback->report (DetectorEventType::noBeam, now);
                        reset ();
                        return;
                        __enable_irq ();
                }
                /* else */ if (beamState == BeamState::absent && currentState == PwmState::low) {

                        beamState = BeamState::present;
                        __disable_irq ();
                        callback->report (DetectorEventType::beamRestored, now);
                        reset ();
                        return;
                        __enable_irq ();
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Trigger detection during steady state.                                   */
        /*--------------------------------------------------------------------------*/

        // If there's no noise which would trigger onEdge and thus force a check.
        if (now - lastSignalChange >= minTriggerEvent1Us) { // Steady for minTriggerEvent1Us or more

                debugPin (false);
                bool longHighState{};
                bool longLowState{};

                if (currentState == PwmState::low &&                // Correct current state
                    currentHighStateStart < currentLowStateStart && // And correct order of previous pwmStates
                    currentMiddleStateStart < currentHighStateStart) {

                        longHighState = (currentLowStateStart - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = (now - currentLowStateStart) >= minTriggerEvent1Us;
                }

                if (longHighState && longLowState && isBeamClean ()) {
                        __disable_irq ();
                        callback->report (DetectorEventType::trigger, currentHighStateStart);
                        blindState = BlindState::blind;
                        blindStateStart = now;
                        reset ();
                        __enable_irq ();
                }
        }
}

/****************************************************************************/

void EdgeFilter::recalculateConstants ()
{
        __disable_irq ();
        minTriggerEvent1Us = msToResult1us (getConfig ().getMinTreggerEventMs ());
        active = getConfig ().isIrSensorOn ();
        __enable_irq ();
}
