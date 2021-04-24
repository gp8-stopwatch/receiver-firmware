/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "IrTriggerDetector.h"

#ifndef UNIT_TEST
#include "Gpio.h"
#include "UsbHelpers.h"
Gpio consoleRx{GPIOA, GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
#define stateChangePin(x) consoleRx.set (x)

Gpio consoleTx{GPIOA, GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
#define triggerOutputPin() HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_9)

Gpio extTriggerOutput (EXT_TRIGGER_OUTPUT_PORT, EXT_TRIGGER_OUTPUT_PINS, GPIO_MODE_OUTPUT_PP);
Gpio extTriggerOutEnable (EXT_TRIGGER_OUT_ENABLE_PORT, EXT_TRIGGER_OUT_ENABLE_PINS, GPIO_MODE_OUTPUT_PP);

// #define stateChangePin(x)
// #define triggerOutputPin(x)
#include "Container.h"
#else
#define __disable_irq(x) x // NOLINT this is for unit testing
#define __enable_irq(x) x // NOLINT
#define stateChangePin(x)
#define triggerOutputPin(x)
#endif

/****************************************************************************/

void IrTriggerDetector::onEdge (Edge const &e, EdgePolarity pol)
{
#ifdef WITH_BLIND_MANAGER
        if (blindManager->isBlind ()) {
                return;
        }
#endif

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
         * [x] Pokazuje no i.r. (często/co drugi raz).
         * [x] Zaimplementować blind time przed kolejnymi testami.
         * TODO EDIT : chyba jednak nie jest to błąd. Przy dużych zakłóceniach widoczne błędne działanie algorytmu. Screeny w katalogu doc. z
         * dnia 24/02/2021
         * TODO Testy powinny symulowac dwór. Czysty sygnał zakłócamy żarówką 100W. Testy z odbiciami są prostsze (nie trzeba żarówki) ale to nie
         * ten use-case.
         * [x] No beam detection stops working after 2 triggers (start + stop) EDIT due to wrong blindState management
         * [x] There was a opposite situation - there was no ir detected even though IS was not obstructed. Yellow trace was hi even though blue
         * was low. EDIT due to wrong blindState management
         */
        if (/* !queue.empty () && */ queue.getFirstPolarity () == pol) {
                // TODO there should be some bit in some register that would tell me that I've missed this ISR. This would be safer and cleaner
                // to use. Reset queue so it's still full, but pulses are 0 width. This will automatically increase noiseCounter by 2
                queue.push (e);
                queue.push (e);
                // queue.push ({e.getFullTimePoint(), (e.polarity == EdgePolarity::rising) ? (EdgePolarity::falling) : (EdgePolarity::rising)});

                /*
                 * TODO test and decide what to do here. To some extent the device can recover.
                 * Although the screen starts to flicker un such cases.
                 */

                // while (true) {
                // }
        }

        queue.push (e);

        // if (!active || blindState == BlindState::blind) {
        //         return;
        // }

        /*--------------------------------------------------------------------------*/
        /* State transitions depending on last level length.                        */
        /*--------------------------------------------------------------------------*/

        auto &last = queue.back ();
        auto &last1 = queue.back1 ();

        if (queue.getFirstPolarity () == EdgePolarity::rising) {
                if ((last.getTimePoint () - last1.getTimePoint ()) >= minTriggerEvent1Us) { // Long low edge
                        if (pwmState != PwmState::low) {
                                pwmState = PwmState::low;
                                lowStateStart = last1.getFullTimePoint ();
                                stateChangePin (false);
                        }

                        checkForEventCondition (e);
                        return;
                }
        }
        else {
                if ((last.getTimePoint () - last1.getTimePoint ()) >= minTriggerEvent1Us) { // Long high edge
                        if (pwmState != PwmState::high) {
                                pwmState = PwmState::high;
                                highStateStart = last1.getFullTimePoint ();
                                stateChangePin (true);
                        }

                        checkForEventCondition (e);
                        return;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* PWM State transitions depending on dutyCycle and recent level length.    */
        /* Here we analiyze events which are shorter than `minTriggerEvent1Us`.     */
        /*--------------------------------------------------------------------------*/

        // Calculate duty cycle of present slice of the signal
        Result1usLS sliceDuration = queue.back ().getTimePoint () - queue.front ().getTimePoint ();
        Result1usLS cycleTresholdForLow = sliceDuration / DUTY_CYCLE_LOW_DIV;
        Result1usLS cycleTresholdForHigh = sliceDuration / DUTY_CYCLE_HIGH_DIV;
        Result1usLS hiDuration = queue.getFirstDuration (); // We assume for now, that queue.getE0() is rising
        Result1usLS lowDuration = queue.getLastDuration ();

        // Find out which edge in the queue is rising, which falling.
        auto *firstRising = &queue.front ();   // Pointers are smaller than Edge object
        auto *firstFalling = &queue.front1 (); // 1 after front

        // 50% chance that above are correct. If our assumption wasn't right, we swap.
        if (queue.getFirstPolarity () != EdgePolarity::rising) {
                std::swap (firstRising, firstFalling);
                std::swap (hiDuration, lowDuration);
        }

        if (hiDuration > cycleTresholdForHigh) { // PWM of the slice is high
                if (pwmState != PwmState::high) {
                        auto stateChangeTimePoint = firstRising->getFullTimePoint ();

                        // if (resultLS (stateChangeTimePoint - lowStateStart) > minTriggerEvent1Us / 20) {
                        pwmState = PwmState::high;
                        highStateStart = stateChangeTimePoint;
                        stateChangePin (true);
                        // }

                        /*
                         * Here, the state change was caused by something shorter than the minTriggerEvent1Us,
                         * so we treat this as a noise.
                         */
                        ++noiseCounter;
                }
        }
        // else if (lowDuration >= cycleTresholdForLow) { // PWM of the slice is low (low duration was for long time)
        else if (hiDuration <= cycleTresholdForLow) { // PWM of the slice is low (low duration was for long time)
                if (pwmState != PwmState::low) {
                        auto stateChangeTimePoint = firstFalling->getFullTimePoint ();

                        // if (resultLS (stateChangeTimePoint - highStateStart) > minTriggerEvent1Us / 20) {
                        pwmState = PwmState::low;
                        lowStateStart = stateChangeTimePoint;
                        stateChangePin (false);
                        // }

                        ++noiseCounter;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Check trigger event conditions.                                          */
        /*--------------------------------------------------------------------------*/

        checkForEventCondition (e);
}

/****************************************************************************/

void IrTriggerDetector::checkForEventCondition (Edge const &e)
{
        if (highStateStart < lowStateStart /* &&    // Correct order of states : first middleState, then High, and at the end low
            middleStateStart < highStateStart */) { // No middle state between high and low
                bool longHighState = resultLS (lowStateStart - highStateStart) >= minTriggerEvent1Us;
                bool longLowState = resultLS (e.getFullTimePoint () - lowStateStart) >= minTriggerEvent1Us;

                if (longHighState && longLowState) {
                        if (isBeamClean ()) {
                                extTriggerToHigh (e.getTimePoint ());
                                // TODO make protocol a field
                                getProtocol ().sendSynchronization (resultLS (e.getFullTimePoint () - highStateStart));
                                callback->report (DetectorEventType::trigger, highStateStart);
                                triggerOutputPin ();
                                reset (); // To prevent reporting twice
                        }
                }
                else {
                        // ++noiseCounter; // Noise counter analyzes the filtered signal
                }
        }
}

/****************************************************************************/

#ifndef UNIT_TEST
void IrTriggerDetector::run ()
#else
void EdgeFilter::run (Result1us now)
#endif
{
        if (!active) {
                return;
        }

#ifdef WITH_BLIND_MANAGER
        if (blindManager->isBlind ()) {
                return;
        }
#endif

        __disable_irq ();
        auto back = queue.back ();
        // auto back1 = queue.back1 ();
        // auto lastDuration = queue.getLastDuration ();
        auto firstPolarity = queue.getFirstPolarity ();
        auto currentState = pwmState;

#ifndef UNIT_TEST
        auto now = stopWatch.getTimeFromIsr ();
#endif

        extTriggerToLowIf (resultLS (now));
        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Steady state detection, pwmState correction.                             */
        /*--------------------------------------------------------------------------*/

        auto lastSignalChangeTimePoint = back.getFullTimePoint ();
        auto lastSignalChangeDuration = resultLS (now - lastSignalChangeTimePoint);
        bool lastSignalChangeLongAgo = lastSignalChangeDuration >= minTriggerEvent1Us;

        // UWAGA
        // Zarówno to trzeba uwzględnić, : hiDuration <= cycleTresholdForLow) { // PWM of the slice is low (low duration was for long time)
        // jak I to: Chyba trzeba z tego zrobić funkcję.
        // if (resultLS (stateChangeTimePoint - highStateStart) >
        // minTriggerEvent1Us / 20) {

        if (/* (lastSignalChangeDuration > lastDuration) || */ lastSignalChangeLongAgo) {
                if (firstPolarity == EdgePolarity::rising) {
                        if (currentState != PwmState::high) {
                                __disable_irq ();
                                currentState = pwmState = PwmState::high;
                                highStateStart = lastSignalChangeTimePoint;
                                __enable_irq ();
                                stateChangePin (true);
                        }
                }
                else {
                        if (currentState != PwmState::low) {
                                __disable_irq ();
                                currentState = pwmState = PwmState::low;
                                lowStateStart = lastSignalChangeTimePoint;
                                __enable_irq ();
                                stateChangePin (false);
                        }
                }
        }

        __disable_irq ();
        auto currentHighStateStart = highStateStart;
        auto currentLowStateStart = lowStateStart;
        // auto currentMiddleStateStart = middleStateStart;
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
                        noiseLevel = std::min<uint8_t> ((MAX_NOISE_LEVEL - 1) * currentNoiseCounter / MAX_NOISE_EVENTS_NUMBER_PER_PERIOD,
                                                        (MAX_NOISE_LEVEL - 1))
                                + 1;
                }
                else {
                        // 0 is a special case where there is absolutely no noise. Levels 1-15 are proportional.
                        noiseLevel = 0;
                }

                // print (noiseLevel);
                // print (currentNoiseCounter);
                // usbWrite ("\r\n");

                if (noiseState == NoiseState::noNoise && noiseLevel >= DEFAULT_NOISE_LEVEL_HIGH) {
                        noiseState = NoiseState::noise;
                        // TODO When noise counter is high, turn of the EXTI, so the rest of the code has a chance to run. Then enable it
                        // after 1s TODO button will stop working during this period.
                        __disable_irq ();
                        callback->report (DetectorEventType::noise, now);
                        reset ();
                        __enable_irq ();
                        return;
                }

                /* else */ if (noiseState == NoiseState::noise && noiseLevel <= DEFAULT_NOISE_LEVEL_LOW) {
                        noiseState = NoiseState::noNoise;
                        __disable_irq ();
                        callback->report (DetectorEventType::noNoise, now);
                        reset (/* now */);
                        __enable_irq ();
                        return;
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
                                                        * For that long, that high time to noBeam calculation time (def 1s) is higher
                                                        * (or eq) than dutyCycle (50 by def.)
                                                        */
                    /* 100 * */ (now - currentHighStateStart)
                            >= /* getConfig ().getDutyTresholdPercent () * */ actualNoBeamCalculationPeriod / 2) {

                        beamState = BeamState::absent;
                        __disable_irq ();
                        callback->report (DetectorEventType::noBeam, now);
                        reset ();
                        __enable_irq ();
                        return;
                }
                /* else */ if (beamState == BeamState::absent && currentState == PwmState::low) {

                        beamState = BeamState::present;
                        __disable_irq ();
                        callback->report (DetectorEventType::beamRestored, now);
                        reset ();
                        __enable_irq ();
                        return;
                }
        }

        /*--------------------------------------------------------------------------*/
        /* Trigger detection during steady state.                                   */
        /*--------------------------------------------------------------------------*/

        // If there's no noise which would trigger onEdge and thus force a check.
        if (lastSignalChangeLongAgo) { // Steady for minTriggerEvent1Us or more

                bool longHighState{};
                bool longLowState{};

                if (currentState == PwmState::low &&                // Correct current state
                    currentHighStateStart < currentLowStateStart /* && // And correct order of previous pwmStates
                    currentMiddleStateStart < currentHighStateStart */) {

                        longHighState = resultLS (currentLowStateStart - currentHighStateStart) >= minTriggerEvent1Us;
                        longLowState = resultLS (now - currentLowStateStart) >= minTriggerEvent1Us;
                }

                if (longHighState && longLowState && isBeamClean ()) {
                        __disable_irq ();
                        extTriggerToHigh (resultLS (now));
                        getProtocol ().sendSynchronization (resultLS (now - highStateStart));
                        callback->report (DetectorEventType::trigger, currentHighStateStart);
                        triggerOutputPin ();
                        reset ();
                        __enable_irq ();
                }
        }
}

/****************************************************************************/

void IrTriggerDetector::recalculateConstants ()
{
        __disable_irq ();
        minTriggerEvent1Us = resultLS (msToResult1us (getConfig ().getMinTreggerEventMs ()));
        active = getConfig ().isIrSensorOn ();
        __enable_irq ();
}

/*****************************************************************************/

void IrTriggerDetector::extTriggerToHigh (Result1usLS now)
{
        highTriggerStateStart = now;
        triggerState = ExtTriggerState::high;
        EXTI->IMR &= ~EXT_TRIGGER_INPUT_PINS;
        extTriggerOutEnable = true;
        extTriggerOutput = true;
}

/*--------------------------------------------------------------------------*/

void IrTriggerDetector::extTriggerToLowIf (Result1usLS now)
{
        if (triggerState == ExtTriggerState::high && now - highTriggerStateStart > EXT_TRIGGER_DURATION_US) {
                triggerState = ExtTriggerState::low;
                extTriggerOutput = false;
                extTriggerOutEnable = false;
                EXTI->IMR |= EXT_TRIGGER_INPUT_PINS;
        }
}
