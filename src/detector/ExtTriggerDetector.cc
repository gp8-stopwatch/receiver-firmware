/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ExtTriggerDetector.h"

#ifndef UNIT_TEST
// #include "Gpio.h"
// #include "UsbHelpers.h"
// Gpio consoleRx{GPIOA, GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
// #define stateChangePin(x) consoleRx.set (x)

// Gpio consoleTx{GPIOA, GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
// #define triggerOutputPin() HAL_GPIO_TogglePin (GPIOA, GPIO_PIN_9)

#define stateChangePin(x)
#define triggerOutputPin(x)
#include "Container.h"
#else
#define __disable_irq(x) x // NOLINT this is for unit testing
#define __enable_irq(x) x  // NOLINT
#define stateChangePin(x)
#define triggerOutputPin(x)
#endif

/****************************************************************************/

void ExtTriggerDetector::onEdge (Edge const &e, EdgePolarity pol)
{
        if (queue.getFirstPolarity () == pol) {
                return;
        }

        noiseDetection (e.getFullTimePoint ());
        queue.push (e);

        /*--------------------------------------------------------------------------*/
        /* State transitions depending on last level length.                        */
        /*--------------------------------------------------------------------------*/

        auto &last = queue.back ();
        auto &last1 = queue.back1 ();

        if (queue.getFirstPolarity () == EdgePolarity::rising) {
                if ((last.getTimePoint () - last1.getTimePoint ()) >= EXT_TRIGGER_DURATION_RX_US) { // Long low edge
                        if (pwmState != PwmState::low) {
                                pwmState = PwmState::low;
                                lowStateStart = last1.getFullTimePoint ();
                                stateChangePin (false);
                        }

                        checkForEventCondition (e);
                        return;
                }

                ++noiseCounter;
        }
        else {
                if ((last.getTimePoint () - last1.getTimePoint ()) >= EXT_TRIGGER_DURATION_RX_US) { // Long high edge
                        if (pwmState != PwmState::high) {
                                pwmState = PwmState::high;
                                highStateStart = last1.getFullTimePoint ();
                                stateChangePin (true);
                        }

                        checkForEventCondition (e);
                        return;
                }

                ++noiseCounter;
        }
}

/****************************************************************************/

void ExtTriggerDetector::checkForEventCondition (Edge const &e)
{
        if (highStateStart < lowStateStart /* &&    // Correct order of states : first middleState, then High, and at the end low
            middleStateStart < highStateStart */) { // No middle state between high and low
                bool longHighState = resultLS (lowStateStart - highStateStart) >= EXT_TRIGGER_DURATION_RX_US;
                bool longLowState = resultLS (e.getFullTimePoint () - lowStateStart) >= EXT_TRIGGER_DURATION_RX_US;

                if (longHighState && longLowState) {
                        callback->report (DetectorEventType::trigger, highStateStart);
                        triggerOutputPin ();
                        reset (); // To prevent reporting twice
                }
        }
}

/****************************************************************************/

#ifndef UNIT_TEST
void ExtTriggerDetector::run ()
#else
void ExtTriggerDetector::run (Result1us now)
#endif
{
        __disable_irq ();
        auto back = queue.back ();
        auto firstPolarity = queue.getFirstPolarity ();
        auto currentState = pwmState;

#ifndef UNIT_TEST
        auto now = stopWatch.getTimeFromIsr ();
#endif

        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Steady state detection, pwmState correction.                             */
        /*--------------------------------------------------------------------------*/

        auto lastSignalChangeTimePoint = back.getFullTimePoint ();
        auto lastSignalChangeDuration = resultLS (now - lastSignalChangeTimePoint);
        bool lastSignalChangeLongAgo = lastSignalChangeDuration >= EXT_TRIGGER_DURATION_RX_US;

        if (lastSignalChangeLongAgo) {
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
        __enable_irq ();

        /*--------------------------------------------------------------------------*/
        /* Noise detection + hysteresis                                             */
        /*--------------------------------------------------------------------------*/

        // Has to be run from both places (onEdge ISR and run) becasue it disables that ISR.
        noiseDetection (now);

        /*--------------------------------------------------------------------------*/
        /* Trigger detection during steady state.                                   */
        /*--------------------------------------------------------------------------*/

        // If there's no noise which would trigger onEdge and thus force a check.
        if (lastSignalChangeLongAgo) { // Steady for minTriggerEvent1Us or more

                bool longHighState{};
                bool longLowState{};

                if (currentState == PwmState::low && // Correct current state
                    currentHighStateStart < currentLowStateStart) {

                        longHighState = resultLS (currentLowStateStart - currentHighStateStart) >= EXT_TRIGGER_DURATION_RX_US;
                        longLowState = resultLS (now - currentLowStateStart) >= EXT_TRIGGER_DURATION_RX_US;
                }

                if (longHighState && longLowState) {
                        __disable_irq ();
                        callback->report (DetectorEventType::trigger, currentHighStateStart);
                        triggerOutputPin ();
                        reset ();
                        __enable_irq ();
                }
        }
}

/****************************************************************************/

void ExtTriggerDetector::noiseDetection (Result1us now)
{
        if (now - lastNoiseCalculation >= msToResult1us (NOISE_CALCULATION_PERIOD_MS)) {
                __disable_irq ();
                lastNoiseCalculation = now;
                auto currentNoiseCounter = noiseCounter;
                noiseCounter = 0;
                __enable_irq ();

                auto noiseAction = [this, &now] {
                        EXTI->IMR &= ~EXT_TRIGGER_INPUT_PINS;
                        __disable_irq ();
                        callback->report (DetectorEventType::cableProblem, now);
                        reset ();
                        __enable_irq ();
                };

                auto testingAction = [] { EXTI->IMR |= EXT_TRIGGER_INPUT_PINS; };

                auto noNoiseAction = [this, &now] {
                        __disable_irq ();
                        callback->report (DetectorEventType::cableOk, now);
                        reset (/* now */);
                        __enable_irq ();
                };

                if (noiseState == NoiseState::noNoise) {
                        if (currentNoiseCounter > 100) {
                                noiseState = NoiseState::noise;
                                noiseAction ();
                        }
                }
                else if (noiseState == NoiseState::testing) {
                        if (currentNoiseCounter > 100) {
                                noiseState = NoiseState::noise;
                                noiseAction ();
                        }
                        else if (currentNoiseCounter < 10) {
                                noiseState = NoiseState::noNoise;
                                noNoiseAction ();
                        }
                }
                else if (noiseState == NoiseState::noise) {
                        noiseState = NoiseState::testing;
                        testingAction ();
                }
        }
}