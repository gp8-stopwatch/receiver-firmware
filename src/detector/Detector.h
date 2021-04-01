/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Types.h"
#include <cstdint>
#ifndef UNIT_TEST
#include "StopWatch.h"
#endif

/****************************************************************************/

#ifdef UNIT_TEST
inline auto &getConfig ()
{
        static cfg::Config config;
        return config;
}
#endif

/*****************************************************************************/

enum class EdgePolarity { falling = 0, rising = 1 };

/**
 * Signal edge with polarity and time of occurence.
 */
struct Edge {
        Edge () = default;
        Edge (Result1us const &ft, EdgePolarity pol) : fullTimePoint{ft}, timePoint{resultLS (ft)}, polarity{pol} {}
        // Result1us timePoint{}; // 64 bits
        Result1us fullTimePoint{}; // 64 bits
        Result1usLS timePoint{};   // 32 bits, less significant portion of fullTimePoint. Up to 70 minutes can be stored here.
        EdgePolarity polarity{};
};

/**
 * A slice of a rectangular signal, 3 edges. Sometimes called simply the "slice"
 */
// using EdgeQueue = etl::circular_buffer<Edge, 3>;

/**
 * A slice of a rectangular signal.
 */
class EdgeQueue3 {
public:
        EdgeQueue3 (EdgePolarity firstPolarity)
        {
                if (firstPolarity == EdgePolarity::rising) { // Low edge at the start
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                }
                else {
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                }
        }

        bool empty () const { return _empty; }
        Edge &back () { return e2; }
        Edge &back1 () { return e1; }
        Edge &back2 () { return e0; }

        Edge &front () { return e0; }
        Edge &front1 () { return e1; }

        Edge &getE0 () { return e0; }
        Edge &getE1 () { return e1; }

        void push (Edge const &e)
        {
                e0 = e1;
                e1 = e2;
                e2 = e;
                _empty = false;
        }

        Result1usLS getDurationA () const
        {
                return (e1.timePoint - e0.timePoint) /* + (e3.timePoint - e2.timePoint) + (e5.timePoint - e4.timePoint) */;
        }
        Result1usLS getDurationB () const
        {
                return (e2.timePoint - e1.timePoint) /* + (e4.timePoint - e3.timePoint) + (e6.timePoint - e5.timePoint) */;
        }

private:
        bool _empty{true};
        Edge e0;
        Edge e1;
        Edge e2;
};

/**
 * A slice of a rectangular signal.
 */
class EdgeQueue7 {
public:
        EdgeQueue7 (EdgePolarity firstPolarity)
        {
                if (firstPolarity == EdgePolarity::rising) {
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                }
                else {
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                        push ({0, EdgePolarity::falling});
                        push ({0, EdgePolarity::rising});
                }
        }

        bool empty () const { return _empty; }
        Edge &back () { return e6; }
        Edge &back1 () { return e5; }
        Edge &back2 () { return e4; }

        Edge &front () { return e0; }
        Edge &front1 () { return e1; }

        Edge &getE0 () { return e0; }
        Edge &getE1 () { return e1; }
        // Edge &getE2 () { return e2; }
        // Edge &getE3 () { return e3; }
        // Edge &getE4 () { return e4; }

        void push (Edge const &e)
        {
                e0 = e1;
                e1 = e2;
                e2 = e3;
                e3 = e4;
                e4 = e5;
                e5 = e6;
                e6 = e;
                _empty = false;
        }

        Result1usLS getDurationA () const
        {
                return (e1.timePoint - e0.timePoint) + (e3.timePoint - e2.timePoint) + (e5.timePoint - e4.timePoint);
        }
        Result1usLS getDurationB () const
        {
                return (e2.timePoint - e1.timePoint) + (e4.timePoint - e3.timePoint) + (e6.timePoint - e5.timePoint);
        }

private:
        bool _empty{true};
        Edge e0;
        Edge e1;
        Edge e2;
        Edge e3;
        Edge e4;
        Edge e5;
        Edge e6;
};

using EdgeQueue = EdgeQueue3;

// Warning! Due to optimization reasons, the values below has to be in sync with enum Event
enum class DetectorEventType { trigger = 0, noise = 1, noNoise = 2, noBeam = 3, beamRestored = 4 };

// /// State of the IR beam that can be always queried.
// enum class DetectorStateType { ok, noise, noBeam };

/**
 *
 */
struct IEdgeDetectorCallback {
        virtual void report (DetectorEventType type, Result1us timePoint) = 0;
};

/**
 * Responsible for reacting to the IR sesnor output as fast as possible, minimizing
 * noise impact on the accuracy of the measuremetns, detecting excessive noise levels,
 * detecting IR signal state.
 */
class EdgeFilter {
public:
        enum class PwmState { low = 0, high = 1, middle };

#ifndef UNIT_TEST
        EdgeFilter (PwmState initialState, StopWatch &st)
            : stopWatch{st}, queue{(initialState == PwmState::low) ? (EdgePolarity::rising) : (EdgePolarity::falling)}, pwmState
        {
                initialState
        }
#else
        EdgeFilter (PwmState initialState) : queue{(initialState == PwmState::low) ? (EdgePolarity::rising) : (EdgePolarity::falling)}, pwmState
        {
                initialState
        }
#endif
        {
                recalculateConstants ();
        }

        /// IRQ context
        void onEdge (Edge const &e);

        /// "main" context. As frequently as possible.
#ifndef UNIT_TEST
        void run ();
#else
        void run (Result1us now);
#endif

        void setCallback (IEdgeDetectorCallback *cb) { callback = cb; }

        /// Has to be run after certain configuration parameters were changed.
        void recalculateConstants ();

        /**
         * Noise level from 0 to 15. 0 is 100% clean signal, 0xf is the noisiest.
         * Noise event is everything that is shorter than current minimum trigger event ms.
         */
        uint8_t getNoiseLevel () const { return noiseLevel; }

        /// How often to calculate if noise state has changed.
        static constexpr uint32_t NOISE_CALCULATION_PERIOD_MS = 1000;
        static constexpr uint16_t NO_BEAM_CALCULATION_PERIOD_MS = 1000;

        /*
         * Most severe (the shortest) noise spike we anticipate for.
         * This is not to qualify a signal change as a noise spike or not. This is
         * to compare noise events to the most severe (the shortest) transient signal
         * change we anticipate for. This is only for calculating the noise level.
         *
         * This value depends mostly (or solely) on the IR receiver used and its bandwith
         * (at least this is my understanding).
         */
        static constexpr Result1usLS MIN_NOISE_SPIKE_1US = 100;

        // TODO turn off this EXTI as well. EDIT : button would not work. The button pin should be changed to 0-1 then.
        bool isActive () const { return active; }
        bool isBeamClean () const { return beamState == BeamState::present && noiseState == NoiseState::noNoise; }

private:
#ifndef UNIT_TEST
        StopWatch &stopWatch;
#endif

        void reset ()
        {
                highStateStart /* = middleStateStart */ = lowStateStart;
                pwmState = PwmState::middle;
        }

        /// Final check if we have proper trigger event
        void checkForEventCondition (Edge const &e);

        bool active{};
        Result1usLS minTriggerEvent1Us{};

        /*--------------------------------------------------------------------------*/
        /* Trigger calculations                                                     */
        /*--------------------------------------------------------------------------*/

        EdgeQueue queue;
        IEdgeDetectorCallback *callback{};

        PwmState pwmState;
        Result1us highStateStart{};
        Result1us lowStateStart{};
        // Result1us middleStateStart{};

        /*--------------------------------------------------------------------------*/
        /* Noise calculations                                                       */
        /*--------------------------------------------------------------------------*/
        enum class NoiseState { noNoise, noise };
        NoiseState noiseState{};
        int noiseCounter{};
        Result1us lastNoiseCalculation{};
        uint8_t noiseLevel{};

        /*--------------------------------------------------------------------------*/
        /* NoBeam calculations                                                      */
        /*--------------------------------------------------------------------------*/
        enum class BeamState { present, absent };
        BeamState beamState{};
        Result1us lastBeamStateCalculation{};

        /*--------------------------------------------------------------------------*/
        /* Blind period calculations                                                */
        /*--------------------------------------------------------------------------*/
        enum class BlindState { notBlind, blind };
        BlindState blindState{};
        Result1us blindStateStart{};
};
