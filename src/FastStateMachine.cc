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
        // #if 0
        //         uint8_t i = display->getIcons ();

        //         if (!ir->isBeamPresent ()) {
        //                 display->setIcons (i | IDisplay::MINUS_SIGN);
        //         }
        //         else {
        //                 display->setIcons (i & ~IDisplay::MINUS_SIGN);
        //         }
        // #endif
        switch (state) {
        case INIT:
#ifdef DEBUG_STATES
                debug->print ("i");
#endif
                ready_entryAction ();
                state = WAIT_FOR_BEAM;

                if (protocol->isRemoteStartAndClear ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                }

                if (auto p = protocol->isRemoteStopAndClear (); p.first == true) {
                        state = GP8_STOP;
                        stop_entryAction (p.second);
                }

                break;

        case WAIT_FOR_BEAM:
#ifdef DEBUG_STATES
                debug->print ("w");
#endif

                if (ir->isBeamPresent ()) {
                        state = GP8_READY;
                }

                if (button && button->getPressClear ()) {
                        state = LOOP_READY;
                        ready_entryAction (true);
                }

                if (protocol->isRemoteStartAndClear ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                }

                if (auto p = protocol->isRemoteStopAndClear (); p.first == true) {
                        state = GP8_STOP;
                        stop_entryAction (p.second);
                }

                break;

        case GP8_READY:
#ifdef DEBUG_STATES
                debug->print ("r");
#endif
                if (ir->isBeamInterrupted () || event == Event::testTrigger) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                        protocol->sendStart ();
                }

                if (button && button->getPressClear ()) {
                        state = LOOP_READY;
                        ready_entryAction (true);
                }

                if (protocol->isRemoteStartAndClear ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                }

                if (auto p = protocol->isRemoteStopAndClear (); p.first == true) {
                        state = GP8_STOP;
                        stop_entryAction (p.second);
                }

                break;

        case GP8_RUNNING:
#ifdef DEBUG_STATES
                debug->print ("u");
#endif
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger) && startTimeout.isExpired ()) {
                        state = GP8_STOP;
                        stop_entryAction ({});
                        protocol->sendStop (stopWatch->getTime ());
                }

                if (protocol->isRemoteStartAndClear ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                }

                if (auto p = protocol->isRemoteStopAndClear (); p.first == true) {
                        state = GP8_STOP;
                        stop_entryAction (p.second);
                }

                // Refresh the screen
                display->setTime (stopWatch->getTime ());
                break;

        case GP8_STOP:
#ifdef DEBUG_STATES
                debug->print ("s");
#endif
                if (((ir->isBeamPresent () && ir->isBeamInterrupted ()) || event == Event::testTrigger) && startTimeout.isExpired ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                        protocol->sendStart ();
                }

                if (button && button->getPressClear ()) {
                        state = LOOP_READY;
                        ready_entryAction (true);
                }

                if (protocol->isRemoteStartAndClear ()) {
                        state = GP8_RUNNING;
                        running_entryAction ();
                }

                if (auto p = protocol->isRemoteStopAndClear (); p.first == true) {
                        state = GP8_STOP;
                        stop_entryAction (p.second);
                }

                break;

                // #if 0
                //         case LOOP_READY:
                //                 if (ir->isBeamPresent () && ir->isBeamInterrupted ()) {
                //                         state = LOOP_RUNNING;
                //                         running_entryAction ();
                //                 }

                //                 if (button && button->getPressClear ()) {
                //                         state = HI_CLEAR_READY;
                //                         hiClearReady_entryAction ();
                //                 }

                //                 break;

                //         case LOOP_RUNNING:
                //                 if ((ir->isBeamPresent () && ir->isBeamInterrupted () && startTimeout.isExpired ()) || buttonPendingCopy) {
                //                         state = LOOP_STOP;
                //                         stop_entryAction ({});
                //                 }

                //                 break;

                //         case LOOP_STOP:
                //                 if ((ir->isBeamPresent () && ir->isBeamInterrupted () && startTimeout.isExpired ()) || buttonPendingCopy) {
                //                         state = LOOP_RUNNING;
                //                         running_entryAction ();
                //                 }

                //                 if (button && button->getPressClear ()) {
                //                         state = HI_CLEAR_READY;
                //                         hiClearReady_entryAction ();
                //                 }

                //                 break;

                //         case HI_CLEAR_READY:
                //                 if (button && button->getLongPressClear ()) {
                //                         buzzer->beep (200, 0, 1);
                //                         history->clearHiScore ();
                //                 }

                //                 if (button && button->getPressClear ()) {
                //                         state = RES_CLEAR_READY;
                //                         resultsClearReady_entryAction ();
                //                 }
                //                 break;

                //         case RES_CLEAR_READY:
                //                 if (button && button->getLongPressClear ()) {
                //                         buzzer->beep (200, 0, 1);
                //                         history->clearResults ();
                //                 }

                //                 if (button && button->getPressClear ()) {
                //                         buzzer->beep (10, 10, 1);
                //                         state = INIT;
                //                 }
                //                 break;
                // #endif

        default:
                break;
        }
}

/*****************************************************************************/

void FastStateMachine::ready_entryAction (bool loop)
{
        display->setDots (0);
        display->setTime (0);

        if (loop) {
                buzzer->beep (10, 10, 1);
                display->setIcons (IDisplay::TOP_LEFT_ARROW);
        }
        else {
                display->setIcons (IDisplay::BOTTOM_LEFT_ARROW);
        }
}

/*****************************************************************************/

void FastStateMachine::running_entryAction ()
{
        stopWatch->reset ();
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

        // if (time) {
        //         display->setTime (*time);
        // }
        display->setTime (result);

        int dif = result - history->getHiScore ();

        if (dif < 0) {
                buzzer->beep (1000, 0, 1);
        }
        else {
                int slots = (dif / 50) + 1;

                if (slots > 5) {
                        slots = 5;
                }

                buzzer->beep (70, 50, slots);
        }

        // TODO same here - storing into flash can be to slow to be used in an IRQ
        history->store (result);

        // TODO we are in an IRQ here! Can't do such slow things as printfs.
        // Debug *d = Debug::singleton ();
        // d->printTime (result);
        // d->print ("\n");
}