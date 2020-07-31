/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "CanProtocol.h"
#include "Timer.h"
#include "Types.h"
#include <optional>

class IInfraRedBeam;
class StopWatch;
struct IDisplay;
class Buzzer;
class History;
class Button;
struct ICircullarQueueStorage;
struct IRandomAccessStorage;

/**
 * Events for the state machine
 */
enum class Event {
        timePassed,  /// Every 10ms / 1ms / 100µs
        irTrigger,   /// IR beam interrupted
        testTrigger, /// Test GPIO state changed
        canBusTrigger,
        pause,
        reset, // Use for resume after pause
        noIr,
        irPresent
};

/**
 * Main algorithm.
 */
class FastStateMachine {
public:
        enum State { WAIT_FOR_BEAM, GP8_READY, GP8_RUNNING, GP8_STOP, LOOP_RUNNING, PAUSED };
        enum RemoteBeamState { wait, allOk, someNotOk, noResponse };

        static FastStateMachine *singleton ()
        {
                static FastStateMachine s;
                return &s;
        }

        void run (Event event);
        bool isCounting () const { return state == State::GP8_RUNNING; }

        void setIr (IInfraRedBeam *i) { this->ir = i; }
        void setStopWatch (StopWatch *s) { this->stopWatch = s; }
        void setDisplay (IDisplay *d) { this->display = d; }
        void setBuzzer (Buzzer *b) { this->buzzer = b; }
        void setHistory (History *h) { this->history = h; }
        void setButton (Button *b) { this->button = b; }
        void setCanProtocol (CanProtocol *cp) { protocol = cp; }

private:
        void waitForBeam_entryAction (bool canEvent);
        void ready_entryAction ();
        void running_entryAction (bool canEvent);
        void stop_entryAction (bool canEvent);
        void loop_entryAction (bool canEvent);
        void pause_entryAction ();

        bool isInternalTrigger (Event event) const;
        bool isInternalTriggerAndStartTimeout (Event event) const;
        bool isExternalTrigger (Event event) const;
        RemoteBeamState isRemoteBeamStateOk () const;

        /*--------------------------------------------------------------------------*/

        State state{WAIT_FOR_BEAM};
        IInfraRedBeam *ir{};
        StopWatch *stopWatch{};
        Timer startTimeout;
        Timer loopDisplayTimeout;
        IDisplay *display{};
        Buzzer *buzzer{};
        History *history{};
        Button *button{};
        CanProtocol *protocol{};

        mutable Timer reqRespTimer;
        mutable bool infoRequestSent{};
        bool noIrRequestSent{};
};

/**
 * Glue code
 */
class FastStateMachineProtocolCallback : public IProtocolCallback {
public:
        FastStateMachineProtocolCallback (FastStateMachine &fs) : fastStateMachine{fs} {}
        void onTrigger () override { fastStateMachine.run (Event::canBusTrigger); }
        void onNoIr () override { fastStateMachine.run (Event::noIr); }

private:
        FastStateMachine &fastStateMachine;
};
