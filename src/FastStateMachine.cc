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
#include "InfraRedBeamExti.h"
#include "StopWatch.h"

/*****************************************************************************/

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
                if (ir->isActive () && ir->getBeamState () != IrBeam::present) {
                        state = State::WAIT_FOR_BEAM;
                }

                if (eType == Event::Type::irTrigger ||   // Internal IR (or laser) sensor
                    eType == Event::Type::testTrigger) { // External GPIO trigger
                        beforeLastTime = lastTime;
                        lastTime = event.getTime ();
                }
                else if (eType == Event::Type::canBusStop || // external CAN bus event overrides the testTrigger
                         eType == Event::Type::canBusLoop) { // external CAN bus event overrides the testTrigger
                        beforeLastTime = 0;
                        lastTime = event.getTime ();
                }
                // Event possible only if WITH_CHECK_SENSOR_STATUS is set
                else if (eType == Event::Type::noIr) {
                        state = State::WAIT_FOR_BEAM;
                        // canEvent = true;
                }
                else if (eType == Event::Type::irNoise) {
                        state = State::WAIT_FOR_BEAM;
                        // canEvent = false;
                }
        }

        // Entry actions and transitions distinct for every state.
        switch (state) {
        case WAIT_FOR_BEAM: {
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

                if (ir->isActive () && ir->getBeamState () == IrBeam::noise) {
                        display->setText ("noise ");

                        // TODO
                        // #ifdef WITH_CAN
                        //                         if (protocol != nullptr && !canEvent && !noIrRequestSent) {
                        //                                 protocol->sendNoIr ();
                        //                                 noIrRequestSent = true;
                        //                         }
                        // #endif
                }
                else if (ir->isActive () && ir->getBeamState () == IrBeam::absent) {
                        display->setText ("noi.r.  ");

#ifdef WITH_CAN
                        if (protocol != nullptr && !isCanBusEvent (event) && !noIrRequestSent) {
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

                // The transition
                if ((ir->isActive () && ir->getBeamState () == IrBeam::present) || remoteBeamState == RemoteBeamState::allOk) {
                        state = READY;
                }

        } break;

        case READY: {
                noIrRequestSent = false;
                ready_entryAction ();

                if (isInternalTrigger (event)) {
                        state = RUNNING;
                        running_entryAction (event /*, canEvent */);
                }

                checkCanBusEvents (event);
        } break;

        case RUNNING:
                if (isInternalTrigger (event)) {

                        if (getConfig ().getStopMode () == StopMode::stop) {
                                state = STOP;
                                stop_entryAction (event /* , canEvent */);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (event);
                        }

                        break;
                }

                checkCanBusEvents (event);

                display->setTime (stopWatch->getTime () - lastTime,
                                  getConfig ().getResolution ()); // Refresh the screen (shows the time is running)
                break;

        case STOP:
                if (isInternalTrigger (event)) {
                        state = RUNNING;
                        running_entryAction (event /* , canEvent */);
                }

                checkCanBusEvents (event);
                break;

        case LOOP_RUNNING:
                if (isInternalTrigger (event)) {
                        loop_entryAction (event);
                }

                checkCanBusEvents (event);

                if (loopDisplayTimeout.isExpired ()) {
                        display->setTime (stopWatch->getTime () - lastTime,
                                          getConfig ().getResolution ()); // Refresh the screen (shows the time is running)
                }

                break;

        case State::PAUSED:
                // pause_entryAction ();
                break;

        default:
                break;
        }
}

/*****************************************************************************/

bool FastStateMachine::isInternalTrigger (Event event) const
{
        return ((ir->getBeamState () == IrBeam::present && ir->isBeamInterrupted ()) || event.getType () == Event::Type::irTrigger);
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

void FastStateMachine::running_entryAction (Event event /* , bool canEvent */)
{
        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (protocol != nullptr && !isCanBusEvent (event)) {
                protocol->sendTrigger (Message::START, lastTime);
        }
#endif

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (100, 0, 1);
#endif
}

/*****************************************************************************/

void FastStateMachine::stop_entryAction (Event event /* , bool canEvent */)
{
        /*--------------------------------------------------------------------------*/
        /* Local time stuff                                                         */
        /*--------------------------------------------------------------------------*/

        Result result = lastTime - beforeLastTime;
        display->setTime (result, getConfig ().getResolution ());

        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (protocol != nullptr && !isCanBusEvent (event)) {
                protocol->sendTrigger (Message::STOP, result);
        }
#endif

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (70, 50, 3);
#endif

#ifdef WITH_HISTORY
        if (history != nullptr) {
                history->store (result);
        }
#endif
}

/*****************************************************************************/

void FastStateMachine::loop_entryAction (Event event /* , bool canEvent */)
{
        /*--------------------------------------------------------------------------*/
        /* Local time stuff                                                         */
        /*--------------------------------------------------------------------------*/

        Result result = lastTime - beforeLastTime;
        display->setTime (result, getConfig ().getResolution ());
        loopDisplayTimeout.start (LOOP_DISPLAY_TIMEOUT);

        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (!isCanBusEvent (event) && protocol != nullptr) {
                protocol->sendTrigger (Message::LOOP, result);
        }
#endif

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (70, 50, 2);
#endif

#ifdef WITH_HISTORY
        if (history != nullptr) {
                history->store (result);
        }
#endif
}

/****************************************************************************/

void FastStateMachine::checkCanBusEvents (Event event)
{
        auto eType = event.getType ();

        if (eType == Event::Type::canBusStart) {
                state = RUNNING;
                running_entryAction (event);
        }
        else if (eType == Event::Type::canBusStop) {
                state = STOP;
                stop_entryAction (event);
        }
        else if (eType == Event::Type::canBusLoop) {
                state = LOOP_RUNNING;
                loop_entryAction (event);
        }
}