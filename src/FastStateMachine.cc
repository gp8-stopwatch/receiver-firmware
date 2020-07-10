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
#include "CanProtocol.h"
#include "Container.h"
#include "Debug.h"
#include "History.h"
#include "IDisplay.h"
#include "InfraRedBeamModulated.h"
#include "StopWatch.h"
//#define DEBUG_STATES 1

#ifdef DEBUG_STATES
#warning "DEBUG_STATES is ON - fast state machine operation may be disturbed."
#endif

/*****************************************************************************/

void FastStateMachine::run (Event event)
{
        // Global transition
        if (event == Event::pause) {
                state = State::PAUSED;
        }
        // Global transition
        else if (event == Event::reset) {
                state = State::INIT;
        }
        /// Global transition for every state other than PAUSE
        else if (state != PAUSED && ir->isActive () && !ir->isBeamPresent ()) {
                state = State::WAIT_FOR_BEAM;
        }

        switch (state) {
        case INIT: {
#ifdef DEBUG_STATES
                debug->print ("i");
#endif
                state = WAIT_FOR_BEAM;
                auto remote = protocol->getLastRemoteStopTime ();

                if (event == Event::canBusStart) {
                        if (remote == 0) {
                                state = GP8_RUNNING;
                                running_entryAction (true);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (true, remote);
                        }
                }

                if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (protocol->getLastRemoteStopTime ());
                }
        } break;

        case WAIT_FOR_BEAM: {
#ifdef DEBUG_STATES
                debug->print ("w");
#endif

                waitForBeam_entryAction ();

                if (ir->isBeamPresent ()) {
                        state = GP8_READY;
                }

                auto remote = protocol->getLastRemoteStopTime ();

                if (event == Event::canBusStart) {
                        if (remote == 0) {
                                state = GP8_RUNNING;
                                running_entryAction (true);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (true, remote);
                        }
                }

                if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (remote);
                }
        } break;

        case GP8_READY: {
#ifdef DEBUG_STATES
                debug->print ("r");
#endif
                ready_entryAction ();

                if (ir->isBeamInterrupted () || event == Event::testTrigger || event == Event::irTrigger) {
                        state = GP8_RUNNING;
                        running_entryAction (false);
                        protocol->sendStart ();
                }

                auto remote = protocol->getLastRemoteStopTime ();

                if (event == Event::canBusStart) {
                        if (remote == 0) {
                                state = GP8_RUNNING;
                                running_entryAction (true);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (true, remote);
                        }
                }

                if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (remote);
                }
        } break;

        case GP8_RUNNING: {
#ifdef DEBUG_STATES
                debug->print ("u");
#endif
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {

                        if (getConfig ().stopMode == StopMode::stop) {
                                state = GP8_STOP;
                                stop_entryAction ({});
                                protocol->sendStop (stopWatch->getTime ());
                        }
                        else { // Loop
                                state = LOOP_RUNNING;
                                loop_entryAction (false, {});
                        }

                        break;
                }

                auto remote = protocol->getLastRemoteStopTime ();

                if (event == Event::canBusStart) {
                        if (remote == 0) {
                                state = GP8_RUNNING;
                                running_entryAction (true);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (true, remote);
                        }
                }

                if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (remote);
                        break;
                }

                // Refresh the screen
                display->setTime (stopWatch->getTime (), getConfig ().resolution);
        } break;

        case GP8_STOP: {
#ifdef DEBUG_STATES
                debug->print ("s");
#endif
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {
                        state = GP8_RUNNING;
                        running_entryAction (false);
                        protocol->sendStart ();
                }

                auto remote = protocol->getLastRemoteStopTime ();

                if (event == Event::canBusStart) {
                        if (remote == 0) {
                                state = GP8_RUNNING;
                                running_entryAction (true);
                        }
                        else {
                                state = LOOP_RUNNING;
                                loop_entryAction (true, remote);
                        }
                }

                if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (remote);
                }
        } break;

        case LOOP_RUNNING:
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {
                        loop_entryAction (false, {});
                }

                if (event == Event::canBusStop || event == Event::canBusStart) {
                        loop_entryAction (true, protocol->getLastRemoteStopTime ());
                }

                if (loopDisplayTimeout.isExpired ()) {
                        // Refresh the screen
                        display->setTime (stopWatch->getTime (), getConfig ().resolution);
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

void FastStateMachine::waitForBeam_entryAction ()
{
        if (!ir->isBeamPresent ()) {
                display->setDot (0);
                display->setText (" noI.R. ");
        }
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction (bool loop)
{
        // display->setDots (0);
        display->setTime (0, getConfig ().resolution);

        // if (loop) {
        //         buzzer->beep (10, 10, 1);
        //         display->setIcons (IDisplay::TOP_LEFT_ARROW);
        // }
        // else {
        //         display->setIcons (IDisplay::BOTTOM_LEFT_ARROW);
        // }
}

/*****************************************************************************/

void FastStateMachine::running_entryAction (bool canStart)
{
        stopWatch->reset (canStart);
        stopWatch->start ();
        buzzer->beep (100, 0, 1);
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
}

/*****************************************************************************/

void FastStateMachine::stop_entryAction (std::optional<uint32_t> time)
{
        stopWatch->stop ();
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
        uint32_t result = (time) ? (*time) : (stopWatch->getTime ());
        display->setTime (result, getConfig ().resolution);

        if (history != nullptr) {
                // int dif = result - history->getHiScore ();

                // if (dif < 0) {
                //         buzzer->beep (1000, 0, 1);
                // }
                // else {
                //         int slots = (dif / 50) + 1;

                //         if (slots > 5) {
                //                 slots = 5;
                //         }

                buzzer->beep (70, 50, 3);
                history->store (result);
        }
}

/****************************************************************************/

void FastStateMachine::pause_entryAction () { stopWatch->stop (); }

/*****************************************************************************/

void FastStateMachine::loop_entryAction (bool canStart, std::optional<uint32_t> time)
{
        uint32_t result = (time) ? (*time) : (stopWatch->getTime ());
        stopWatch->stop ();

        if (!canStart) {
                protocol->sendStart (stopWatch->getTime ());
        }

        stopWatch->reset (canStart);
        stopWatch->start ();
        buzzer->beep (100, 0, 1);
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
        loopDisplayTimeout.start (LOOP_DISPLAY_TIMEOUT);

        display->setTime (result, getConfig ().resolution);

        if (history != nullptr) {
                buzzer->beep (70, 50, 2);

                /*
                 * "This means that code or data fetches cannot be made while a program/erase operation is ongoing". p.57
                 * In loop mode saving the result to flash causes 200µs error. In normal (non-loop) mode there is no such
                 * problem as the timer stops. This problem is hard to resolve. One option would be to use external flash
                 * (for storing the results), the other would be to move parts of the code (ISRs) to RAM?
                 */
                // history->store (result);
        }
}