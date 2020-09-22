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
        bool canEvent{};

        // Global transitions (the same for every state)
        if (event.getType () == Event::Type::pause) {
                state = State::PAUSED;
        }
        else if (event.getType () == Event::Type::reset) {
                state = State::WAIT_FOR_BEAM;
        }

        // Global except for the PAUSED state
        if (state != PAUSED) {
                if (ir->isActive () && ir->getBeamState () != IrBeam::present) {
                        state = State::WAIT_FOR_BEAM;
                }

                // Event possible only if WITH_CHECK_SENSOR_STATUS is set
                if (event.getType () == Event::Type::noIr) {
                        state = State::WAIT_FOR_BEAM;
                        canEvent = true;
                }

                if (event.getType () == Event::Type::irNoise) {
                        state = State::WAIT_FOR_BEAM;
                        canEvent = false;
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
                        if (protocol != nullptr && !canEvent && !noIrRequestSent) {
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

                if (isInternalTrigger (event) || (canEvent = isExternalTrigger (event))) {
                        state = RUNNING;
                        running_entryAction (event /*, canEvent */);
                }
        } break;

        case RUNNING:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {

                        if (getConfig ().getStopMode () == StopMode::stop) {
                                state = STOP;
                                stop_entryAction (event /* , canEvent */);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (event, canEvent);
                        }

                        break;
                }

                display->setTime (stopWatch->getTime () - lastTime,
                                  getConfig ().getResolution ()); // Refresh the screen (shows the time is running)
                break;

        case STOP:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {
                        state = RUNNING;
                        running_entryAction (event /* , canEvent */);
                }

                break;

        case LOOP_RUNNING:
                if (isInternalTriggerAndStartTimeout (event) || (canEvent = isExternalTrigger (event))) {
                        loop_entryAction (event, canEvent);
                }

                if (loopDisplayTimeout.isExpired ()) {
                        display->setTime (stopWatch->getTime () - lastTime,
                                          getConfig ().getResolution ()); // Refresh the screen (shows the time is running)
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
        return ((ir->getBeamState () == IrBeam::present && ir->isBeamInterrupted ()) || event.getType () == Event::Type::testTrigger
                || event.getType () == Event::Type::irTrigger);
}

/*****************************************************************************/

bool FastStateMachine::isInternalTriggerAndStartTimeout (Event event) const
{
        return isInternalTrigger (event); /* && startTimeout.isExpired (); */
}

/*****************************************************************************/

bool FastStateMachine::isExternalTrigger (Event event) const { return event.getType () == Event::Type::canBusTrigger; }

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
        /* Local time stuff                                                         */
        /*--------------------------------------------------------------------------*/

        // Result correction{};

        // TODO not tested
        // if (canEvent) {
        //         // correction = StopWatch::CAN_LATENCY_CORRECTION /* + event.getTime () */ + protocol->getLastRemoteStopTime ();
        //         // TODO StopWatch::CAN_LATENCY_CORRECTION + protocol->getLastRemoteStopTime () should be stored in the event.getTime
        //         lastTime = StopWatch::CAN_LATENCY_CORRECTION /* + event.getTime () */ + protocol->getLastRemoteStopTime ();
        // }
        // else {
        //         // correction = (stopWatch->getTime () - event.getTime ());
        lastTime = event.getTime ();

        // }

        // stopWatch->set (correction);
        // stopWatch->start ();
        // startTimeout.start (getConfig ().getBlindTime ());

        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (protocol != nullptr && event.getType () != Event::Type::canBusTrigger) {
                protocol->sendTrigger (lastTime);
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

        // stopWatch->stop ();
        // stopWatch->substract (stopWatch->getTime () - event.getTime ());
        // uint32_t canTime = (protocol != nullptr) ? (protocol->getLastRemoteStopTime ()) : (0UL);
        // uint32_t result = (canEvent) ? (canTime) : (event.getTime () - lastTime);
        Result result = event.getTime () - lastTime;

        display->setTime (result, getConfig ().getResolution ());

        // startTimeout.start (getConfig ().getBlindTime ());

        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (protocol != nullptr && event.getType () != Event::Type::canBusTrigger) {
                protocol->sendTrigger (result);
        }
#endif

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (70, 50, 3);
#endif

#ifdef WITH_FLASH
        if (history != nullptr) {
                history->store (result);
        }
#endif
}

/*****************************************************************************/

void FastStateMachine::loop_entryAction (Event event, bool canEvent)
{
        /*--------------------------------------------------------------------------*/
        /* Local time stuff                                                         */
        /*--------------------------------------------------------------------------*/

        Result now = event.getTime ();
        Result result = now - lastTime;
        lastTime = now;

        // // stopWatch->substract (correction);
        // uint32_t canTime = (protocol != nullptr) ? (protocol->getLastRemoteStopTime ()) : (0UL);
        // uint32_t result = (canEvent) ? (canTime) : (event.getTime () - lastTime);
        // // stopWatch->set (correction);

        // if (canEvent) {
        //         // correction = StopWatch::CAN_LATENCY_CORRECTION /* + event.getTime () */ + protocol->getLastRemoteStopTime ();
        //         // TODO StopWatch::CAN_LATENCY_CORRECTION + protocol->getLastRemoteStopTime () should be stored in the event.getTime
        //         lastTime = StopWatch::CAN_LATENCY_CORRECTION /* + event.getTime () */ + protocol->getLastRemoteStopTime ();
        // }
        // else {
        //         // correction = (stopWatch->getTime () - event.getTime ());
        //         lastTime = event.getTime ();
        // }

        display->setTime (result, getConfig ().getResolution ());

        // startTimeout.start (getConfig ().getBlindTime ());
        loopDisplayTimeout.start (LOOP_DISPLAY_TIMEOUT);

        /*--------------------------------------------------------------------------*/
        /* CAN bus stuff                                                            */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_CAN
        if (!canEvent && protocol != nullptr) {
                protocol->sendTrigger (result);
        }
#endif

        /*--------------------------------------------------------------------------*/
        /* Bookkeeping                                                              */
        /*--------------------------------------------------------------------------*/

#ifdef WITH_SOUND
        buzzer->beep (70, 50, 2);
#endif

#ifdef WITH_FLASH
        if (history != nullptr) {
                history->store (result);
        }
#endif
}

/****************************************************************************/

void FastStateMachine::pause_entryAction ()
{ /* stopWatch->stop (); */
}
