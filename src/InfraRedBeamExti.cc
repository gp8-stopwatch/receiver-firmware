/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "InfraRedBeamExti.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"

static constexpr int MIN_TIME_BETWEEN_EVENTS_MS = 10;
static constexpr int NOISE_CLEAR_TIMEOUT_MS = 1000;
static constexpr int NOISE_EVENTS_CRITICAL = 5;
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

void InfraRedBeamExti::onExti (IrBeam state)
{
        if (!active) {
                return;
        }

        if (state == IrBeam::absent) {

                if (beamPresentTimer.elapsed () < MIN_TIME_BETWEEN_EVENTS_MS) {
                        beamPresentTimer.start (NO_IR_DETECTED_MS); // Force no IR
                        beamNoiseTimer.start (NOISE_CLEAR_TIMEOUT_MS);

                        // We simply ingnore spurious noise events if they don't occur too frequently.
                        if (++noiseEventCounter > NOISE_EVENTS_CRITICAL) {
                                lastState = IrBeam::noise;

                                // HAL_NVIC_DisableIRQ (IR_IRQn);
                                HAL_NVIC_SetPriority (TIM15_IRQn, IR_EXTI_PRIORITY, 0);    // Priorities are inverted
                                HAL_NVIC_SetPriority (IR_IRQn, DISPLAY_TIMER_PRIORITY, 0); // Priorities are inverted

                                fStateMachine->run (Event::irNoise);
                        }
                }
                else {
                        beamPresentTimer.start (NO_IR_DETECTED_MS);
                        lastState = IrBeam::absent;
                        sendEvent (fStateMachine, Event::irTrigger);
                }
        }
        else {
                if (beamPresentTimer.elapsed () < MIN_TIME_BETWEEN_EVENTS_MS) {
                        beamNoiseTimer.start (NOISE_CLEAR_TIMEOUT_MS);
                        // lastState = IrBeam::noise;
                }
                else {
                        beamPresentTimer.start (0);
                        lastState = IrBeam::present;
                }
        }
}

/*****************************************************************************/

void InfraRedBeamExti::run ()
{
        // Clear noise status after NOISE_CLEAR_TIMEOUT_MS or more ms (but it can be restored later during this call).
        if (beamNoiseTimer.isExpired ()) {
                noiseEventCounter = 0;

                if (lastState == IrBeam::noise) {
                        // HAL_NVIC_EnableIRQ (IR_IRQn);
                        HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
                        HAL_NVIC_SetPriority (IR_IRQn, IR_EXTI_PRIORITY, 0);

                        lastState = IrBeam::absent;
                }
        }
}
