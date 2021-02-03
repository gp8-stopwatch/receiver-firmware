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

struct IInfraRedBeam;
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
class Event {
public:
        enum class Type {
                timePassed,  /// Every 10ms / 1ms / 100µs
                irTrigger,   /// IR beam interrupted
                testTrigger, /// External trigger (M-LVDS) GPIO state changed
                canBusLoop,  /// Peripheral device reported the restart of the LOOP state.
                pause,
                reset, // Use for resume after pause
                noIr,
                irPresent,
                irNoise
        };

        Event (Type t, Result r = 0) : type{t}, time{r} {}

        Type getType () const { return type; }
        Result getTime () const { return time; }

private:
        Type type;
        Result time;
};

/**
 * Main algorithm.
 */
class FastStateMachine {
public:
        enum State { WAIT_FOR_BEAM, READY, RUNNING, STOP, LOOP_RUNNING, PAUSED };
        enum RemoteBeamState { wait, allOk, someNotOk, noResponse };

        static FastStateMachine *singleton ()
        {
                static FastStateMachine s;
                return &s;
        }

        void run (Event event);
        bool isCounting () const { return state == State::RUNNING || state == State::LOOP_RUNNING; }

        void setIr (IInfraRedBeam *i) { this->ir = i; }
        void setStopWatch (StopWatch *s) { this->stopWatch = s; }
        void setDisplay (IDisplay *d) { this->display = d; }
        void setBuzzer (Buzzer *b) { this->buzzer = b; }
        void setHistory (History *h) { this->history = h; }
        void setCanProtocol (CanProtocol *cp) { protocol = cp; }

private:
        void ready_entryAction ();
        void running_entryAction (Event event /* , bool canEvent */);
        void stop_entryAction (Event event /* , bool canEvent */);
        void loopStop_entryAction (Event event /* , bool canEvent */);
        // void pause_entryAction ();

        // void checkCanBusEvents (Event event);
        bool isInternalTrigger (Event event) const;
        bool isCanBusEvent (Event event) const
        {
                auto eType = event.getType ();
                return (/* eType == Event::Type::canBusStart || eType == Event::Type::canBusStop || */ eType == Event::Type::canBusLoop);
        }

        // bool isExternalTrigger (Event event) const;
        RemoteBeamState isRemoteBeamStateOk () const;

        /*--------------------------------------------------------------------------*/

        State state{WAIT_FOR_BEAM};
        IInfraRedBeam *ir{};
        StopWatch *stopWatch{};
        Timer loopDisplayTimeout;
        IDisplay *display{};
        Buzzer *buzzer{};
        History *history{};
        CanProtocol *protocol{};
        Result lastTime{};
        Result beforeLastTime{};

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
        void onMessage (Message msg, Result time) override
        {

                switch (msg) {
                        // case Message::START:
                        //         fastStateMachine.run ({Event::Type::canBusStart, time});
                        //         break;

                        // case Message::STOP:
                        //         fastStateMachine.run ({Event::Type::canBusStop, time});
                        //         break;

                case Message::LOOP:
                        fastStateMachine.run ({Event::Type::canBusLoop, time});
                        break;

#ifdef WITH_CHECK_SENSOR_STATUS
                case Message::NO_IR:
                        fastStateMachine.run ({Event::Type::noIr, time});
                        break;
#endif

                default:
                        break;
                }
        }

private:
        FastStateMachine &fastStateMachine;
};
