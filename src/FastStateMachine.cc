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

#ifdef DEBUG_STATES
#warning "DEBUG_STATES is ON - fast state machine operation may be disturbed."
#endif

/*****************************************************************************/

void FastStateMachine::run (Event event)
{
        auto canTime = protocol->getLastRemoteStopTime ();

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
                // Czanbus events are handled in every state
                else if (event == Event::canBusStart) {
                        state = GP8_RUNNING;
                        running_entryAction (true);
                }
                else if (event == Event::canBusLoopStart) {
                        state = LOOP_RUNNING;
                        loop_entryAction (true, canTime);
                }
                else if (event == Event::canBusStop) {
                        state = GP8_STOP;
                        stop_entryAction (true, canTime);
                }
        }

        // Entry actions and transitions distinct for every state.
        switch (state) {
        case WAIT_FOR_BEAM:
                waitForBeam_entryAction ();

                if (ir->isBeamPresent () || !ir->isActive ()) {
                        state = GP8_READY;
                }

                break;

        case GP8_READY:
                ready_entryAction ();

                if (ir->isBeamInterrupted () || event == Event::testTrigger || event == Event::irTrigger) {
                        state = GP8_RUNNING;
                        running_entryAction (false);
                        protocol->sendStart ();
                }
                break;

        case GP8_RUNNING:
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {

                        if (getConfig ().stopMode == StopMode::stop) {
                                state = GP8_STOP;
                                stop_entryAction (false, {});
                        }
                        else { // Loop
                                state = LOOP_RUNNING;
                                loop_entryAction (false, {});
                        }

                        break;
                }

                // Refresh the screen
                display->setTime (stopWatch->getTime (), getConfig ().resolution);
                break;

        case GP8_STOP:
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {
                        state = GP8_RUNNING;
                        running_entryAction (false);
                        protocol->sendStart ();
                }

                break;

        case LOOP_RUNNING:
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger || event == Event::irTrigger)
                    && startTimeout.isExpired ()) {
                        loop_entryAction (false, {});
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
        if (ir->isActive () && !ir->isBeamPresent ()) {
                display->setDots (0);
                display->setText (" noI.R. ");
        }
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction (bool loop) { display->setTime (0, getConfig ().resolution); }

/*****************************************************************************/

void FastStateMachine::running_entryAction (bool canStart)
{
        stopWatch->reset (canStart);
        stopWatch->start ();
        buzzer->beep (100, 0, 1);
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
}

/*****************************************************************************/

void FastStateMachine::stop_entryAction (bool canEvent, std::optional<uint32_t> time)
{
        stopWatch->stop ();
        startTimeout.start (BEAM_INTERRUPTION_EVENT);
        uint32_t result = (time) ? (*time) : (stopWatch->getTime ());

        if (!canEvent) {
                protocol->sendStop (result);
        }

        display->setTime (result, getConfig ().resolution);

        if (history != nullptr) {
                buzzer->beep (70, 50, 3);
                history->store (result);
        }
}

/*****************************************************************************/

void FastStateMachine::loop_entryAction (bool canEvent, std::optional<uint32_t> time)
{
        stopWatch->stop ();
        uint32_t result = (time) ? (*time) : (stopWatch->getTime ());

        if (!canEvent) {
                protocol->sendLoopStart (result);
        }

        stopWatch->reset (canEvent);
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

/****************************************************************************/

void FastStateMachine::pause_entryAction () { stopWatch->stop (); }
