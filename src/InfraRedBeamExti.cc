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

// #include <array>
#include <etl/queue.h>

/*****************************************************************************/

void printDebug (IrBeam state, bool external);
void sendEvent (FastStateMachine *fStateMachine, Event ev);

/*****************************************************************************/

/*
 * 2 clean trigger events which would result in time beeing measured between A and B.
 *
 *        +-----+                                       +-----------+      IrBeam::triggerRising (high)
 *        |     |                                       |           |
 *        |     |                                       |           |
 *        |     |                                       |           |
 *        |     |                                       |           |
 * -------+     +---------------------------------------+           +----  IrBeam::triggerFalling (low)
 *        A                                             B
 *
 * Single event, a closeup. General view of what can happen in a noisy environment (the worst case):
 *
 *   +      +---+    +      +---+---+  +--+---+  +-+--+        +     +  + +   +
 *   |      |   |    |      |   |   |  |  |   |  | |  |        |     |  | |   |
 *   |      |   |    |      |   |   |  |  |   |  | |  |        |     |  | |   |
 *   |      |   |    |      |   |   |  |  |   |  | |  |        |     |  | |   |
 *   |      |   |    |      |   |   |  |  |   |  | |  |        |     |  | |   |
 * --+------+   +----+------+   +   +--+  +   +--+ +  +--------+-----+--+-+---+----+
 *   a      b   c           D   e   f  g              H                            I
 *
 * - a: Positive transient noise event resulting from a DC ambient light like sun or bright lamps.
 * - b-c: Long positive noise event (rising and falling edges close to each other).
 * - D: Trigger event rising edge
 * - e: Negative transiuent noise event (sun, reflections, weak IR)
 * - f-g: Long negative noise event
 * - H: Trigger event falling edge.
 * - I: Event complete .
 *
 * Definitions:
 * Trigger event: Certain chain of consecutive signal edges.
 *
 * Noise event: an edge which occured in *n* ms after previous edge, where n is less than MIN_TRIGGER_EVENT_MS.
 * If the edge is rising (like b), then the event is negative (event a-b). If the edge is falling (c), the
 * nois event is positive (b-c).
 *
 * Noise sources:
 * - Ambient light.
 * - IR reflections.
 * - Weak IR signal.
 * - Transient voltages induced in PCB traces (and LVDS cable?). Those are the fastest.
 */
void InfraRedBeamExti::onExti (IrBeam state, bool external)
{
        if ((!active && !external) || lastState == IrBeam::noise) {
                return;
        }

        Result1us now = stopWatch->getTime ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint;
        lastIrChangeTimePoint = now;

        if (state == IrBeam::triggerRising) {
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

/**
 * This is run from an IRQ with highest priority.
 * Here word "event" means an edge in the IR or EXT signal.
 * This function tracks the current state of the beam, and calculates
 * (with minimal latency and maximum accuracy) other invariants. It cares so
 * the follwoing are always up to date:
 * - state
 * - dutyCycle (how much the trigger signal was high in %).
 *
 */
void InfraRedBeamExti::onExti2 (IrBeam event, bool external)
{
        if (!active) {
                return;
        }

        Result1us now = stopWatch->getTime ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint;
        lastIrChangeTimePoint = now;

        switch (state) {
        case State::idle: // Waiting for an event. Only trigger rising is interesting after device turned on.
                if (event == IrBeam::triggerRising) {
                        triggerRisingEdgeTime = now;
                        lastIrChangeTimePoint = now;
                        state = (external) ? (State::extTriggerHigh) : (State::irTriggerHigh);
                }
                // else if (event == IrBeam::triggerFalling) {
                //         triggerFallingEdgeTime = now;
                //         state = (external) ? (State::extTriggerLow) : (State::irTriggerLow);
                // }
                break;

        case State::irTriggerLow:
                if (event == IrBeam::triggerRising && !external) {
                        triggerRisingEdgeTime = now;
                        triggerLowPeriod += lastIrChangeDuration;
                        state = State::irTriggerHigh;
                }
                break;

        case State::irTriggerHigh:
                if (event == IrBeam::triggerFalling && !external) {
                        triggerFallingEdgeTime = now;
                        triggerHighPeriod += lastIrChangeDuration;
                        state = State::irTriggerLow;
                }
                break;

        case State::extTriggerLow:
                if (event == IrBeam::triggerRising && external) {
                        triggerRisingEdgeTime = now;
                        triggerLowPeriod += lastIrChangeDuration;
                        state = State::extTriggerHigh;
                }
                break;

        case State::extTriggerHigh:
                if (event == IrBeam::triggerFalling && external) {
                        triggerFallingEdgeTime = now;
                        triggerHighPeriod += lastIrChangeDuration;
                        state = State::extTriggerLow;
                }
                break;

        case State::noise:
        case State::noBeam:
        default:
                break;
        }
}

/**
 * This is run from an IRQ which has lower priority than what InfraRedBeamExti::onExti is run from.
 * There is no preemption possible (it can be postponed until exti finishes its work though).
 *
 * Q: Why this has to be run from the main thread (non-IRQ)?
 * A: IRQ routine cannot be preempted on this MCU, so instead reducing latency I increased it!
 * Better quickly copy relevant balues in a critical section, and use them in a code that can be
 * safely preempted.
 */
void InfraRedBeamExti::run2 ()
{
        if (!refreshTimer.isExpired ()) { // Run this every 100ms or so.
                return;
        }

        refreshTimer.start (REFRESH_TIMEOUT_MS);
        Result1us now = stopWatch->getTime ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint; // For how long the trigger is low? (without any change)
        Result1us envelope = triggerHighPeriod + triggerLowPeriod;    // Period between State::idle and last State::xxxTriggerFalling
        float dutyCycle = float (triggerHighPeriod) / float (envelope);

        /*
         * Detecting correct start/stop event for the FastStateMachine.
         * Trigger signal is low -> IR signal is present no object is crossing the barrier.
         */
        if ((state == State::irTriggerLow || state == State::extTriggerLow) && // State is correct for barrier interrupted event to be genrated
            lastIrChangeDuration >= msToResult1 (MIN_TRIGGER_LOW_STEADY_TIME_MS)) { // Trigger signal was low for certain time

                /*
                 * So trigger was low and steady for a certain amount of time
                 * and if the following additional conditions are met, we have
                 * a correct barrier interrupted event.
                 */
                if (envelope >= msToResult1 (MIN_TIME_BETWEEN_EVENTS_MS) && // Envelope no too short
                    envelope < msToResult1 (DEFAULT_BLIND_TIME_MS) &&       // Envelope not too long
                    dutyCycle >= MIN_DUTY_CYCLE &&                          // Duty cycle higher than ...
                    blindTimeout.isExpired ()) {                            // Blind time has to be taken into account as well.

                        // Pass the event to the FastStateMachine
                        sendEvent (fStateMachine, {Event::Type::irTrigger, *triggerRisingEdgeTime});
                        blindTimeout.start (getConfig ().getBlindTime ());
                }
        }

        /*
         * Detecting the noIr event. The state machine has to be reset to work
         * properly afterwards.
         */
        else if (envelope >= msToResult1 (NO_IR_DETECTED_MS)) {
                state = State::noBeam;
        }
        /*
         * Detecting and reporting noise. The state machine has to be reset to work
         * properly afterwards.
         */
        else if (true) {
                state = State::noise;
        }
}

/*
dead lock ! - próbuje wypisać result.

HAL_GetTick@0x0800e072 (/HAL_GetTick.cdasm:2)
HAL_Delay@0x0800e094 (/HAL_Delay.cdasm:11)
usbWrite@0x0801203c (/usbWrite.cdasm:67)
printResult(unsigned long, ResultDisplayStyle)@0x08012dc2 (Unknown Source:0)
FastStateMachine::run(Event)@0x080035a4 (Unknown Source:0)
InfraRedBeamExti::run()@0x08003b86 (Unknown Source:0)
SysTick_Handler@0x080027bc (/SysTick_Handler.cdasm:45)
<signal handler called>@0xfffffff9 (Unknown Source:0)
getBuzzer()@0x080057d2 (Unknown Source:0)
main@0x08002cc2 (/main.cdasm:198)
*/

/**
 * This is run from an IRQ which has lower priority than what InfraRedBeamExti::onExti is run from.
 * There is no preemption possible (it can be postponed until exti finishes its work though).
 */
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
        // __disable_irq ();
        Result1us lastIrChangeDuration = now - lastIrChangeTimePoint;
        bool triggerRisingEdgeTimeSet = bool (triggerRisingEdgeTime);
        bool duty = irAbsentPeriod > irPresentPeriod;
        auto lastStateCopy = lastState;
        Result1us envelope = (triggerRisingEdgeTimeSet) ? (triggerFallingEdgeTime - *triggerRisingEdgeTime) : (0);
        // __enable_irq ();

        // EVENT detection. Looking for correct envelope
        if (triggerRisingEdgeTimeSet && lastIrChangeDuration >= msToResult1 (MIN_TIME_BETWEEN_EVENTS_MS)
            && lastStateCopy == IrBeam::triggerFalling) {

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

                        // __disable_irq ();
                        auto triggerRisingEdgeTimeCopy = *triggerRisingEdgeTime;
                        // __enable_irq ();

                        sendEvent (fStateMachine, {Event::Type::irTrigger, triggerRisingEdgeTimeCopy});
                        blindTimeout.start (getConfig ().getBlindTime ());
                }

                extTriggerOutput.set (false);
                extTriggerOutEnable.set (false);
                EXTI->IMR |= EXT_TRIGGER_INPUT_PINS;

                // __disable_irq ();
                // After correct event has been detected, we reset everything.
                irPresentPeriod = irAbsentPeriod = triggerFallingEdgeTime = 0;
                triggerRisingEdgeTime.reset ();
                // __enable_irq ();
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
                if (state == IrBeam::triggerRising) {
                        print ("0");
                }
                else {
                        print ("1");
                }
        }
        else {
                if (state == IrBeam::triggerRising) {
                        print ("a");
                }
                else {
                        print ("p");
                }
        }
}