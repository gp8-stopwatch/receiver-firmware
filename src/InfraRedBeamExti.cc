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

/*****************************************************************************/

void sendEvent (FastStateMachine *fStateMachine, Event ev)
{
#ifdef TEST_TRIGGER_MOD_2
        static int i{};
        if (++i % 2 == 0) {
                fStateMachine->run (ev);
        }
#else
        fStateMachine->run (ev);
#endif
}

/*****************************************************************************/

void InfraRedBeamExti::onExti ()
{
        IrBeam state = getPinState ();

        if (!active || lastState == IrBeam::noise) {
                return;
        }

        Result now = stopWatch->getTime ();

        if (state == IrBeam::absent) {

                beamPresentTimer.start (NO_IR_DETECTED_MS);
                lastState = IrBeam::absent;
                // We don't know if this is a valid trigger event or noise, so we store current time for later.

                if (!triggerRisingEdgeTime) {
                        triggerRisingEdgeTime = now;
                }
                else {
                        irPresentPeriod += now - lastIrChange;
                }
        }
        else {
                lastState = IrBeam::present;
                triggerFallingEdgeTime = now;

                if (triggerRisingEdgeTime) {
                        irAbsentPeriod += now - lastIrChange;
                }

                // IR was restored, but the time it was off was too short, which means noise event
                if (now - lastIrChange < MIN_TIME_BETWEEN_EVENTS_MS * 100) {

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
                // IR was restored, and the time it was off is valid to qualify it as an trigger event.
                else {
                        beamPresentTimer.start (0);
                }
        }

        // Rising or falling edge on IR pin.
        lastIrChange = now;
}

/*****************************************************************************/

void InfraRedBeamExti::run ()
{
        Result now = stopWatch->getTime ();

        // EVENT detection. Looking for correct envelope
        if (triggerRisingEdgeTime && now - lastIrChange >= MIN_TIME_BETWEEN_EVENTS_MS * 100 && getPinState () == IrBeam::present) {

                Result envelope = triggerFallingEdgeTime - *triggerRisingEdgeTime;

                if (envelope >= MIN_TIME_BETWEEN_EVENTS_MS * 100 && envelope < DEFAULT_BLIND_TIME_MS * 100 && irAbsentPeriod > irPresentPeriod) {
                        sendEvent (fStateMachine, {Event::Type::irTrigger, *triggerRisingEdgeTime});
                }

                irPresentPeriod = irAbsentPeriod = triggerFallingEdgeTime = 0;
                triggerRisingEdgeTime.reset ();
        }

        // NOISE. This runs every NOISE_CLEAR_TIMEOUT_MS and checks if noise events number was exceeded. Then the counter is cleared.
        // Crude but simple.
        if (beamNoiseTimer.isExpired ()) {
                beamNoiseTimer.start (NOISE_CLEAR_TIMEOUT_MS);
                noiseEventCounter = 0;

                if (lastState == IrBeam::noise) {
#ifdef WITH_DISPLAY
                        HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
                        HAL_NVIC_SetPriority (IR_IRQn, IR_EXTI_PRIORITY, 0);
#endif
                        // reset
                        lastState = getPinState ();
                        beamPresentTimer.start (0);
                }
        }
}
