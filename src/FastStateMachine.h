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
#include "detector/BlindManager.h"
#include "detector/ExtTriggerDetector.h"
#include "detector/IrTriggerDetector.h"
#include <etl/queue.h>
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
        // Warining : for optimization reasons, the numbers below has to be in sync with DetectorEventType
        enum class Type {
                irTrigger = 0, /// IR beam interrupted
                noise = 1,
                noNoise = 2,
                noBeam = 3,
                beamRestored = 4,
                cableProblem = 5, // Noise on LVDS bus
                cableOk = 6,
                externalTrigger, /// External trigger (M-LVDS) GPIO state changed.
                timePassed,      /// Every 10ms / 1ms / 100µs
                // canBusLoop,  /// Peripheral device reported the restart of the LOOP state.
                pause,
                reset, // Use for resume after pause
        };

        Event (Type t = Type::timePassed, Result1us r = 0) : type{t}, time{r} {}

        Type getType () const { return type; }
        Result1us getTime () const { return time; } /// TODO change name to timepoint

private:
        Type type;
        Result1us time;
};

/**
 * Queue of events.
 */
using EventQueue = etl::queue<Event, 8, etl::memory_model::MEMORY_MODEL_SMALL>;

/**
 * Main algorithm.
 */
class FastStateMachine {
public:
        enum class State { WAIT_FOR_BEAM, READY, RUNNING, STOP, LOOP_RUNNING, PAUSED, NOISE, NO_BEAM, CABLE_PROBLEM };
        enum class RemoteBeamState { wait, allOk, someHasNoise, someHasNoBeam, noResponse, allInactive };

        void run (Event event);
        bool isCounting () const { return state == State::RUNNING || state == State::LOOP_RUNNING; }

        void setIrTriggerDetector (IrTriggerDetector *i) { this->ir = i; }
        void setExtTriggerDetector (ExtTriggerDetector *i) { this->ext = i; }
        void setStopWatch (StopWatch *s) { this->stopWatch = s; }
        void setDisplay (IDisplay *d) { this->display = d; }
        void setBuzzer (Buzzer *b) { this->buzzer = b; }
        void setHistory (History *h) { this->history = h; }
        void setCanProtocol (CanProtocol *cp) { protocol = cp; }
        void setBlindManager (BlindManager *b) { blindManager = b; }

private:
        void ready_entryAction ();
        void running_entryAction (Event event);
        void stop_entryAction (Event event);
        void loopStop_entryAction (Event event);

        static bool isTrigger (Event event)
        {
                return event.getType () == Event::Type::externalTrigger || event.getType () == Event::Type::irTrigger;
        }

        static bool isExternalTrigger (Event event) { return event.getType () == Event::Type::externalTrigger; }

        // bool isExternalTrigger (Event event) const;
        RemoteBeamState isRemoteBeamStateOk () const;

        /*--------------------------------------------------------------------------*/

        State state{State::WAIT_FOR_BEAM};
        IrTriggerDetector *ir{};
        ExtTriggerDetector *ext{};
        StopWatch *stopWatch{};
        Timer loopDisplayTimeout;
        IDisplay *display{};
        Buzzer *buzzer{};
        History *history{};
        CanProtocol *protocol{};
        Result1us lastTime{};
        Result1us beforeLastTime{};
        BlindManager *blindManager{};

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
        void onMessage (Message msg /* , Result1us time */) override
        {

                switch (msg) {
#ifdef IS_CAN_MASTER
                case Message::NO_BEAM:
                        fastStateMachine.run ({Event::Type::noBeam});
                        break;

                case Message::NOISE:
                        fastStateMachine.run ({Event::Type::noise});
                        break;
#endif

                default:
                        break;
                }
        }

private:
        FastStateMachine &fastStateMachine;
};
