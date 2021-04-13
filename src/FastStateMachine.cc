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
#include "StopWatch.h"
#include "UsbHelpers.h"

/**
 * Q&A
 * Q: Why not clear the stopwatch timer before new measurement? That would increase accuracy.
 * A: Because only one participant could be contesting at the same time then.
 */
void FastStateMachine::run (Event event)
{
        // Temp variable for storing information if the trigger was interanal or external.
        auto eType = event.getType ();

        // Global transitions (the same for every state)
        if (eType == Event::Type::pause) {
                state = State::PAUSED;
        }
        else if (eType == Event::Type::reset) {
                state = State::WAIT_FOR_BEAM;
        }

        // Global except for the PAUSED state
        if (state != PAUSED) {
                if (ir->isActive () && !ir->isBeamClean ()) {
                        state = State::WAIT_FOR_BEAM;
                }

                if (eType == Event::Type::irTrigger ||       // Internal IR (or laser) sensor
                    eType == Event::Type::externalTrigger) { // External GPIO trigger
                        if (blindManager->isSeeing ()) {
                                beforeLastTime = lastTime;
                                lastTime = event.getTime ();
                                blindManager->start ();
                        }
                        else {
                                return;
                        }
                }
                // Event possible only if WITH_CHECK_SENSOR_STATUS is set
                // else if (eType == Event::Type::noBeam || eType == Event::Type::noise) {
                //         state = State::WAIT_FOR_BEAM;
                // }
                else if (eType == Event::Type::noBeam) {
                        state = State::NO_BEAM;
                }
                else if (eType == Event::Type::noise) {
                        state = State::NOISE;
                }
        }

        // Entry actions and transitions distinct for every state.
        switch (state) {
        case WAIT_FOR_BEAM: {
                /*
#ifdef WITH_CHECK_SENSOR_STATUS
                // The entry action
                auto remoteBeamState = isRemoteBeamStateOk ();

                // Query was sent, we are waiting for the response
                if (remoteBeamState == RemoteBeamState::wait) {
                        break;
                }
#else
                auto remoteBeamState = RemoteBeamState::noResponse;
#endif

                if (event.getType () == Event::Type::noise) {
                        display->setText ("noise ");

                        // TODO
                        // #ifdef WITH_CAN
                        //                         if (protocol != nullptr && !canEvent && !noIrRequestSent) {
                        //                                 protocol->sendNoIr ();
                        //                                 noIrRequestSent = true;
                        //                         }
                        // #endif
                }
                else if (event.getType () == Event::Type::noBeam) {
                        display->setText ("noi.r.  ");

#ifdef WITH_CAN
                        if (protocol != nullptr && !isExternalTrigger (event) && !noIrRequestSent) {
                                protocol->sendNoIr ();
                                noIrRequestSent = true;
                        }
#endif
                }
#ifdef WITH_CHECK_SENSOR_STATUS
                else if (remoteBeamState == RemoteBeamState::someNotOk) {
                        display->setText ("nobeam");
                }
                else if (remoteBeamState == RemoteBeamState::noResponse) {
                        display->setText ("blind ");
                }
#endif
*/
                // The transition
                if ((ir->isActive () && ir->isBeamClean ()) /* || remoteBeamState == RemoteBeamState::allOk */) {
                        state = READY;
                }

        } break;

        case NOISE:
                display->setText ("noise ");

                // TODO
                // #ifdef WITH_CAN
                //                         if (protocol != nullptr && !canEvent && !noIrRequestSent) {
                //                                 protocol->sendNoIr ();
                //                                 noIrRequestSent = true;
                //                         }
                // #endif

                if (event.getType () == Event::Type::noNoise) {
                        state = WAIT_FOR_BEAM;
                }

                break;

        case NO_BEAM:
                display->setText ("noi.r.  ");

                if (event.getType () == Event::Type::beamRestored) {
                        state = WAIT_FOR_BEAM;
                }

#ifdef WITH_CAN
                if (protocol != nullptr && !isExternalTrigger (event) && !noIrRequestSent) {
                        protocol->sendNoIr ();
                        noIrRequestSent = true;
                }
#endif

                break;

        case READY: {
                noIrRequestSent = false;
                ready_entryAction ();

                /*
                 * Even if the mode was selected to "loop", the very first counting is performed in the "running" mode.
                 * This is to avoid showing the first result which is not there.
                 */
                if (isTrigger (event)) {
                        state = RUNNING;
                        running_entryAction (event);
                }

        } break;

        case RUNNING:
                // Refresh the screen (shows the time is running). In an event of the stop_entryAction this will be re-set again.
                display->setTime (result1To10 (stopWatch->getTime () - lastTime), getConfig ().getResolution ());

                if (isTrigger (event)) {
                        if (getConfig ().getStopMode () == StopMode::stop) {
                                state = STOP;
                        }
                        else {
                                state = LOOP_RUNNING;
                        }

                        loopStop_entryAction (event);
                }

                break;

        case STOP:
                if (isTrigger (event)) {
                        state = RUNNING;
                        running_entryAction (event);
                }

                break;

        case LOOP_RUNNING:
                if (loopDisplayTimeout.isExpired ()) {
                        display->setTime (result1To10 (stopWatch->getTime () - lastTime),
                                          getConfig ().getResolution ()); // Refresh the screen (shows the time is running)
                }

                if (isTrigger (event)) {
                        loopStop_entryAction (event);
                }

                break;

        case State::PAUSED:
        default:
                break;
        }
}

/*****************************************************************************/

FastStateMachine::RemoteBeamState FastStateMachine::isRemoteBeamStateOk () const
{
        if (!reqRespTimer.isExpired ()) {
                return RemoteBeamState::wait;
        }

        if (infoRequestSent) {
                RemoteBeamState ret = RemoteBeamState::allOk;
                auto &resp = protocol->getInfoRespDataCollection ();

                for (auto &periph : resp) {
                        if (periph.beamState == BeamState::no) {
                                ret = RemoteBeamState::someNotOk;
                        }
                }

                if (resp.empty ()) {
                        ret = RemoteBeamState::noResponse;
                }

                infoRequestSent = false;
                return ret;
        }

        protocol->sendInfoRequest ();
        reqRespTimer.start (RESPONSE_WAIT_TIME_MS);
        infoRequestSent = true;
        return RemoteBeamState::wait;
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction () { display->setTime (0, getConfig ().getResolution ()); }

/*****************************************************************************/

void FastStateMachine::running_entryAction (Event event)
{
        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (100, 0, 1);
#endif
}

/*****************************************************************************/

void FastStateMachine::loopStop_entryAction (Event event)
{
        /*--------------------------------------------------------------------------*/
        /* Local time stuff                                                         */
        /*--------------------------------------------------------------------------*/

        auto result = result1To10 (lastTime - beforeLastTime);
        display->setTime (result, getConfig ().getResolution ());
        loopDisplayTimeout.start (LOOP_DISPLAY_TIMEOUT);

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_USB
        if (auto rd = getConfig ().getAutoDisplayResult (); rd != ResultDisplayStyle::none) {
                printResult (result, rd);
                usbWrite ("\r\n");
        }
#endif

#ifdef WITH_SOUND
        buzzer->beep (70, 50, (state == LOOP_RUNNING) ? (2) : (3));
#endif

#ifdef WITH_HISTORY
        if (history != nullptr) {
                history->store (result);
        }
#endif
}
