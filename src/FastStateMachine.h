/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
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
class CanProtocol;

/**
 * Events for the state machine
 */
enum class Event {
        timePassed,  /// Every 10ms / 1ms / 100µs
        irTrigger,   /// IR beam interrupted
        testTrigger, /// Test GPIO state changed
        canBusStart,
        canBusStop,
        pause,
        reset, // Use for resume after pause
};

class FastStateMachine {
public:
        enum State { WAIT_FOR_BEAM, GP8_READY, GP8_RUNNING, GP8_STOP, LOOP_RUNNING, PAUSED };

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
        void waitForBeam_entryAction ();
        void ready_entryAction (bool loop = false);
        void running_entryAction (bool canStart);
        void stop_entryAction (std::optional<uint32_t> time);
        // void hiClearReady_entryAction ();
        // void resultsClearReady_entryAction ();
        void pause_entryAction ();
        void loop_entryAction (bool canStart, std::optional<uint32_t> time);

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

        bool loop = true; // Temporary - use Config::stopMode instead.
};
