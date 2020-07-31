/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "FastStateMachine.h"
#include "Button.h"
#include "Buzzer.h"
#include "Container.h"
#include "Debug.h"
#include "History.h"
#include "IDisplay.h"
#include "InfraRedBeamModulated.h"
#include "StopWatch.h"

/*****************************************************************************/

void FastStateMachine::run (Event event)
{
        // Temp variable for storing information if the trigger was interanal or external.
        bool canEvent{};

        // Global transitions (the same for every state)
        if (event == Event::pause) {
                state = State::PAUSED;
        }
        else if (event == Event::reset) {
                state = State::WAIT_FOR_BEAM;
        }

        // Global except for the PAUSED state
        if (state != PAUSED) {
                if (ir->isActive () && !ir->isBeamPresent ()) {
                        state = State::WAIT_FOR_BEAM;
                }

                // Event possible only if WITH_CAN_TRIGGER is set
                if (event == Event::noIr) {
                        state = State::WAIT_FOR_BEAM;
                        canEvent = true;
                }
        }

        // Entry actions and transitions distinct for every state.
        switch (state) {
        case WAIT_FOR_BEAM:
                waitForBeam_entryAction (canEvent);

                // Event possible only if WITH_CAN_TRIGGER is set
                if (ir->isBeamPresent () /* || !ir->isActive () */            // 1. Check if local IR is OK
                    || event == Event::irPresent /* TODO isRemoteIrPresent */ // 2. Check if all peripherals are have IR.
                ) {
                        state = GP8_READY;
                }

                break;

        case GP8_READY: {
                ready_entryAction ();

                if (isInternalTrigger (event) || (canEvent = isExternalTrigger (event))) {
                        state = GP8_RUNNING;
                        running_entryAction (canEvent);
                }
        } break;

        case GP8_RUNNING:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {

                        if (getConfig ().stopMode == StopMode::stop) {
                                state = GP8_STOP;
                                stop_entryAction (canEvent);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (canEvent);
                        }

                        break;
                }

                display->setTime (stopWatch->getTime (), getConfig ().resolution); // Refresh the screen (shows the time is running)
                break;

        case GP8_STOP:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {
                        state = GP8_RUNNING;
                        running_entryAction (canEvent);
                }

                break;

        case LOOP_RUNNING:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {
                        loop_entryAction (canEvent);
                }

                if (loopDisplayTimeout.isExpired ()) {
                        display->setTime (stopWatch->getTime (), getConfig ().resolution); // Refresh the screen (shows the time is running)
                }

                break;

        case State::PAUSED:
                pause_entryAction ();
                break;

        default:
                break;
        }
}

/*****************************************************************************/

bool FastStateMachine::isInternalTrigger (Event event) const
{
        return ((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger);
}

/*****************************************************************************/

bool FastStateMachine::isInternalTriggerAndStartTimeout (Event event) const { return isInternalTrigger (event) && startTimeout.isExpired (); }

/*****************************************************************************/

bool FastStateMachine::isExternalTrigger (Event event) const
{
#ifdef WITH_CAN_TRIGGER
        return event == Event::canBusTrigger;
#else
        return false;
#endif
}

/*****************************************************************************/

void FastStateMachine::waitForBeam_entryAction (bool canEvent)
{
        if (ir->isActive () && !ir->isBeamPresent ()) {
                display->setText (" noI.R. ");

#ifdef WITH_CAN
                // if (protocol != nullptr && !canEvent) {
                //         // protocol->sendNoIr (); // TODO send only once
                // }
#endif
        }
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction ()
{
        display->setTime (0, getConfig ().resolution);

#ifdef WITH_CAN
        // if (protocol != nullptr) {
        //         protocol->sendIrPresent ();
        // }
#endif
}

/*****************************************************************************/

void FastStateMachine::running_entryAction (bool canEvent)
{
        stopWatch->reset (canEvent);
        stopWatch->start ();

#ifdef WITH_SOUND
        buzzer->beep (100, 0, 1);
#endif
        startTimeout.start (BEAM_INTERRUPTION_EVENT);

        if (!canEvent && protocol != nullptr) {
#ifdef WITH_CAN
                protocol->sendTrigger (0);
#endif
        }
}

/*****************************************************************************/

void FastStateMachine::stop_entryAction (bool canEvent)
{
        stopWatch->stop ();
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
        uint32_t canTime = (protocol != nullptr) ? (protocol->getLastRemoteStopTime ()) : (0UL);
        uint32_t result = (canEvent) ? (canTime) : (stopWatch->getTime ());

        if (!canEvent && protocol != nullptr) {
#ifdef WITH_CAN
                protocol->sendTrigger (result);
#endif
        }

        display->setTime (result, getConfig ().resolution);

        if (history != nullptr) {
#ifdef WITH_SOUND
                buzzer->beep (70, 50, 3);
#endif
#ifdef WITH_FLASH
                history->store (result);
#endif
        }
}

/*****************************************************************************/

void FastStateMachine::loop_entryAction (bool canEvent)
{
        stopWatch->stop ();
        uint32_t canTime = (protocol != nullptr) ? (protocol->getLastRemoteStopTime ()) : (0UL);
        uint32_t result = (canEvent) ? (canTime) : (stopWatch->getTime ());

        if (!canEvent && protocol != nullptr) {
#ifdef WITH_CAN
                protocol->sendTrigger (result);
#endif
        }

        stopWatch->reset (canEvent);
        stopWatch->start ();
#ifdef WITH_SOUND
        buzzer->beep (100, 0, 1);
#endif
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
        loopDisplayTimeout.start (LOOP_DISPLAY_TIMEOUT);

        display->setTime (result, getConfig ().resolution);

        if (history != nullptr) {
#ifdef WITH_SOUND
                buzzer->beep (70, 50, 2);
#endif
#ifdef WITH_FLASH
                history->store (result);
#endif
        }
}

/****************************************************************************/

void FastStateMachine::pause_entryAction () { stopWatch->stop (); }
