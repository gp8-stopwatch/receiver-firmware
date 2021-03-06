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
        if (state != State::PAUSED) {
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
                // Event possible only if IS_CAN_MASTER is set
                // else if (eType == Event::Type::noBeam || eType == Event::Type::noise) {
                //         state = State::WAIT_FOR_BEAM;
                // }
                else if (eType == Event::Type::noBeam) {
                        state = State::NO_BEAM;
                }
                else if (eType == Event::Type::noise) {
                        state = State::NOISE;
                }
                else if (eType == Event::Type::cableProblem) {
                        state = State::CABLE_PROBLEM;
                }
        }

        // Entry actions and transitions distinct for every state.
        switch (state) {
        case State::WAIT_FOR_BEAM: {

                if (!reqRespTimer.isExpired ()) {
                        break;
                }

                reqRespTimer.start (RESPONSE_WAIT_TIME_MS);

#ifdef IS_CAN_MASTER
                // The entry action
                auto remoteBeamState = isRemoteBeamStateOk ();

                // Query was sent, we are waiting for the response
                if (remoteBeamState == RemoteBeamState::wait) {
                        break;
                }
#else
                auto remoteBeamState = RemoteBeamState::noResponse;
#endif

                //                 if (event.getType () == Event::Type::noise) {
                //                         display->setText ("noise ");

                //                         // TODO
                //                         // #ifdef WITH_CAN
                //                         //                         if (protocol != nullptr && !canEvent && !noIrRequestSent) {
                //                         //                                 protocol->sendNoIr ();
                //                         //                                 noIrRequestSent = true;
                //                         //                         }
                //                         // #endif
                //                 }
                //                 else if (event.getType () == Event::Type::noBeam) {
                //                         display->setText ("noi.r.  ");

                // #ifdef WITH_CAN
                //                         if (protocol != nullptr && !isExternalTrigger (event) && !noIrRequestSent) {
                //                                 protocol->sendNoBeam ();
                //                                 noIrRequestSent = true;
                //                         }
                // #endif
                //                 }
                // #ifdef IS_CAN_MASTER
                // #endif

                if (!ext->isBeamClean ()) {
                        display->setText ("cable ");
                        break;
                        // No can frame this time, because everybody on the LVDS bus can see the noise.
                }

                if (ir->isActive ()) {
                        if (ir->getNoiseState () == NoiseState::noise) {
                                display->setText ("noise ");

                                if (protocol != nullptr) {
                                        protocol->sendNoise ();
                                }

                                break;
                        }

                        if (ir->getBeamState () == BeamState::absent) {
                                display->setText ("nobeam");

                                if (protocol != nullptr) {
                                        protocol->sendNoBeam ();
                                }

                                break;
                        }
                }

                if (remoteBeamState == RemoteBeamState::someHasNoise) {
                        display->setText ("noise ");
                        break;
                }

                if (remoteBeamState == RemoteBeamState::someHasNoBeam) {
                        display->setText ("nobeam");
                        break;
                }

                // No possbility of detecting trigger.
                if (!ir->isActive () && (remoteBeamState == RemoteBeamState::noResponse || remoteBeamState == RemoteBeamState::allInactive)) {
                        display->setText ("blind ");
                        break;
                }

                // // The transition
                // if (ir->isActive () && ir->isBeamClean () && ext->isBeamClean ()                                        // Local state is all
                // OK.
                //     && (remoteBeamState == RemoteBeamState::allOk || remoteBeamState == RemoteBeamState::noResponse)) { // Remote is OK
                state = State::READY;
                // }
        } break;

        case State::NOISE:
                // display->setText ("noise ");

                // TODO
                // #ifdef WITH_CAN
                //                         if (protocol != nullptr && !canEvent && !noIrRequestSent) {
                //                                 protocol->sendNoIr ();
                //                                 noIrRequestSent = true;
                //                         }
                // #endif

                // if (event.getType () == Event::Type::noNoise) {
                state = State::WAIT_FOR_BEAM;
                // }

                break;

        case State::CABLE_PROBLEM:
                // display->setText ("cable ");

                // We do not need to send this over CAN bus, because all would see the LVDS noise anyway.
                // if (event.getType () == Event::Type::cableOk) {
                state = State::WAIT_FOR_BEAM;
                // }

                break;

        case State::NO_BEAM:
                // display->setText ("noi.r.  ");

                // if (event.getType () == Event::Type::beamRestored) {
                state = State::WAIT_FOR_BEAM;
                // }

                // #ifdef WITH_CAN
                //                 if (protocol != nullptr && !isExternalTrigger (event) && !noIrRequestSent) {
                //                         protocol->sendNoBeam ();
                //                         noIrRequestSent = true;
                //                 }
                // #endif

                break;

        case State::READY: {
                noIrRequestSent = false;
                ready_entryAction ();

                /*
                 * Even if the mode was selected to "loop", the very first counting is performed in the "running" mode.
                 * This is to avoid showing the first result which is not there.
                 */
                if (isTrigger (event)) {
                        state = State::RUNNING;
                        running_entryAction (event);
                }

        } break;

        case State::RUNNING:
                // Refresh the screen (shows the time is running). In an event of the stop_entryAction this will be re-set again.
                display->setTime (result1To10 (stopWatch->getTime () - lastTime), getConfig ().getResolution ());

                if (isTrigger (event)) {
                        if (getConfig ().getStopMode () == StopMode::stop) {
                                state = State::STOP;
                        }
                        else {
                                state = State::LOOP_RUNNING;
                        }

                        loopStop_entryAction (event);
                }

                break;

        case State::STOP:
                if (isTrigger (event)) {
                        state = State::RUNNING;
                        running_entryAction (event);
                }

                break;

        case State::LOOP_RUNNING:
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
        // if (!reqRespTimer.isExpired ()) {
        //         return RemoteBeamState::wait;
        // }

        if (infoRequestSent) {
                RemoteBeamState ret = RemoteBeamState::allOk;
                auto &resp = protocol->getInfoRespDataCollection ();
                unsigned int inactiveNum{};

                for (auto &periph : resp) {
                        if (!periph.active) {
                                ++inactiveNum;
                                continue; // All micro are active by default. Other RX-es can be inactive, and that's also OK.
                        }

                        if (periph.beamState == BeamState::absent) {
                                ret = RemoteBeamState::someHasNoBeam;
                                break;
                        }

                        if (periph.noiseState == NoiseState::noise) {
                                ret = RemoteBeamState::someHasNoise;
                                break;
                        }
                }

                if (resp.empty ()) {
                        ret = RemoteBeamState::noResponse;
                }
                else if (inactiveNum == resp.size ()) { // resp.size > 0
                        ret = RemoteBeamState::allInactive;
                }

                infoRequestSent = false;
                return ret;
        }

        protocol->sendInfoRequest ();
        // reqRespTimer.start (RESPONSE_WAIT_TIME_MS);
        infoRequestSent = true;
        return RemoteBeamState::wait;
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction () { display->setTime (0, getConfig ().getResolution ()); }

/*****************************************************************************/

void FastStateMachine::running_entryAction (Event /* event */)
{
        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (100, 0, 1);
#endif
}

/*****************************************************************************/

void FastStateMachine::loopStop_entryAction (Event /* event */)
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
        buzzer->beep (70, 50, (state == State::LOOP_RUNNING) ? (2) : (3));
#endif

#ifdef WITH_HISTORY
        if (history != nullptr) {
                history->store (result);
        }
#endif
}
