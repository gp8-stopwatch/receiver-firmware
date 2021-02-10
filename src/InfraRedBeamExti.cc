/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "InfraRedBeamExti.h"
#include "Container.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"
#include "StopWatch.h"
#include "UsbHelpers.h"

/*****************************************************************************/

void printDebug (IrBeam state, bool external);
void sendEvent (FastStateMachine *fStateMachine, Event ev);

/*****************************************************************************/

/*
       +-----+                                       +-----------+      IrBeam::absent
       |     |                                       |           |
       |     |                                       |           |
       |     |                                       |           |
       |     |                                       |           |
-------+     +---------------------------------------+           +----  IrBeam::present
 */
void InfraRedBeamExti::onExti (IrBeam state, bool external)
{

        if ((!active && !external) || lastState == IrBeam::noise) {
                return;
        }

        Result1us now = stopWatch->getTime ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint;
        lastIrChangeTimePoint = now;

        if (state == IrBeam::absent) {
                if (!triggerRisingEdgeTime) {
                        triggerRisingEdgeTime = now;
                        triggerFallingEdgeTime = now;

                        if (!external) {
                                EXTI->IMR &= ~EXT_TRIGGER_INPUT_PINS;
                                extTriggerOutEnable = true;
                                extTriggerOutput = true;
                        }
                }
                else {
                        irPresentPeriod += lastIrChangeDuration;
                }

                beamPresentTimer.start (NO_IR_DETECTED_MS);

                // if (!external) {
                //         lastState = IrBeam::absent;
                // }
        }
        else {
                triggerFallingEdgeTime = now;

                if (triggerRisingEdgeTime) {
                        irAbsentPeriod += lastIrChangeDuration;
                }

                // if (!external) {
                //         lastState = IrBeam::present;
                // }
                // else if (blindTimeout.isExpired ()) { // external event!
                //                                       // We receive already filtered event, so there's no need to check the envelope.

                //         sendEvent (fStateMachine, {Event::Type::externalTrigger, *triggerRisingEdgeTime});
                //         blindTimeout.start (getConfig ().getBlindTime ());
                //         // Reset the state.
                //         irPresentPeriod = irAbsentPeriod = triggerFallingEdgeTime = 0;
                //         triggerRisingEdgeTime.reset ();
                // }

                // IR was restored, but the time it was off was too short, which means noise spike
                if (lastIrChangeDuration < msToResult1 (MIN_TIME_BETWEEN_EVENTS_MS)) {

                        // We simply ingnore spurious noise events if they don't occur too frequently.
                        if (++noiseEventCounter > NOISE_EVENTS_CRITICAL) {
                                lastState = IrBeam::noise;

#ifdef WITH_DISPLAY
                                HAL_NVIC_SetPriority (TIM15_IRQn, IR_EXTI_PRIORITY, 0);    // Priorities are inverted
                                HAL_NVIC_SetPriority (IR_IRQn, DISPLAY_TIMER_PRIORITY, 0); // Priorities are inverted
#endif

                                fStateMachine->run (Event::Type::irNoise);
                        }
                }
                // IR was restored
                else {
                        beamPresentTimer.start (0);
                }
        }

        lastState = state;
}

/*****************************************************************************/

void InfraRedBeamExti::run ()
{
        if (!refreshTimer.isExpired ()) { // Run this every 100ms or so.
                return;
        }

        refreshTimer.start (REFRESH_TIMEOUT_MS);

        /*
         * Do not disable ALL the IRQs! But how to do it insetad? Some another IRQ with priority?
         * For now i lowered the frequency this methid is invoked with to 10Hz. This minimizes
         * probability of the EXTI IRQ beeing delayed by this critical section blocks.
         */
        Result1us now = stopWatch->getTime ();

        // The whole state has to be retrieved atomically. This is why those strange copies are taken.
        __disable_irq ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint;
        bool triggerRisingEdgeTimeSet = bool (triggerRisingEdgeTime);
        bool duty = irAbsentPeriod > irPresentPeriod;
        auto lastStateCopy = lastState;
        Result1us envelope = (triggerRisingEdgeTimeSet) ? (triggerFallingEdgeTime - *triggerRisingEdgeTime) : (0);
        __enable_irq ();

        // EVENT detection. Looking for correct envelope
        if (triggerRisingEdgeTimeSet && lastIrChangeDuration >= msToResult1 (MIN_TIME_BETWEEN_EVENTS_MS) && lastStateCopy == IrBeam::present) {

                /*
                 * Numerous conditions has to be met to qualify noisy IR signal as a valid event:
                 * - MIN_TIME_BETWEEN_EVENTS_MS has passed since IR was obscured (default 10ms) and then detected again. This is so we know that
                 * the IR has settled.
                 * - Envelope of the IR signal (i.e. the time between first rising IR edge and last falling edge) has to be at least
                 * MIN_TIME_BETWEEN_EVENTS_MS.
                 * - IR has to be absent at least half of the envelope time.
                 */
                if (envelope >= msToResult1 (MIN_TIME_BETWEEN_EVENTS_MS) && envelope < msToResult1 (DEFAULT_BLIND_TIME_MS) && duty
                    && blindTimeout.isExpired ()) {

                        __disable_irq ();
                        auto triggerRisingEdgeTimeCopy = *triggerRisingEdgeTime;
                        __enable_irq ();

                        sendEvent (fStateMachine, {Event::Type::irTrigger, triggerRisingEdgeTimeCopy});
                        blindTimeout.start (getConfig ().getBlindTime ());
                }

                extTriggerOutput.set (false);
                extTriggerOutEnable.set (false);
                EXTI->IMR |= EXT_TRIGGER_INPUT_PINS;

                __disable_irq ();
                // After correct event has been detected, we reset everything.
                irPresentPeriod = irAbsentPeriod = triggerFallingEdgeTime = 0;
                triggerRisingEdgeTime.reset ();
                __enable_irq ();
        }

        // NOISE. This runs every NOISE_CLEAR_TIMEOUT_MS and checks if noise events number was exceeded. Then the counter is cleared.
        // Crude but simple.
        if (beamNoiseTimer.isExpired ()) {
                beamNoiseTimer.start (NOISE_CLEAR_TIMEOUT_MS);
                noiseEventCounter = 0;

                if (lastStateCopy == IrBeam::noise) {
#ifdef WITH_DISPLAY
                        // TODO The priorites are inverted, but they are not inverted back when noise is back to normal
                        // HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
                        // HAL_NVIC_SetPriority (IR_IRQn, IR_EXTI_PRIORITY, 0);
#endif
                        // reset
                        lastState = getPinState ();
                        beamPresentTimer.start (0);
                }
        }
}

/****************************************************************************/

void sendEvent (FastStateMachine *fStateMachine, Event ev)
{
#ifdef TEST_TRIGGER_MOD_2
        static int i{};
        if (++i % 2 == 1) {
                fStateMachine->run (ev);
        }
#else
        fStateMachine->run (ev);
#endif
}

/****************************************************************************/

void printDebug (IrBeam state, bool external)
{
        // For testing
        if (external) {
                if (state == IrBeam::absent) {
                        print ("0");
                }
                else {
                        print ("1");
                }
        }
        else {
                if (state == IrBeam::absent) {
                        print ("a");
                }
                else {
                        print ("p");
                }
        }
}