/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "StopWatch.h"
#include "Types.h"
#include <cstdint>
#include <etl/circular_buffer.h>

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
        Result1us timePoint{};
        EdgePolarity polarity{};
};

/**
 * A slice of a rectangular signal, 3 edges. Sometimes called simply the "slice"
 */
// using EdgeQueue = etl::circular_buffer<Edge, 3>;

/**
 * A slice of a rectangular signal, 3 edges. Sometimes called simply the "slice"
 */
class EdgeQueue {
public:
        bool empty () const { return _empty; }
        Edge &back () { return e2; }
        Edge &front () { return e0; }

        Edge &getE0 () { return e0; }
        Edge &getE1 () { return e1; }
        Edge &getE2 () { return e2; }

        void push (Edge const &e)
        {
                e0 = e1;
                e1 = e2;
                e2 = e;
                _empty = false;
        }

private:
        bool _empty{true};
        Edge e0;
        Edge e1;
        Edge e2;
};

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

        EdgeFilter (PwmState initialState, StopWatch &st) : stopWatch{st}, pwmState{initialState}
        {
                recalculateConstants ();

                if (pwmState == PwmState::low) {
                        queue.push ({0, EdgePolarity::rising});
                        queue.push ({0, EdgePolarity::falling});
                }
                else {
                        // Possibly remove this, and assume that initial state is always low. Then initial checks in onEdge would assure that
                        // edges are in correct order?
                        queue.push ({0, EdgePolarity::falling});
                        queue.push ({0, EdgePolarity::rising});
                        // beamState = BeamState::absent;
                }
        }

        /// IRQ context
        void onEdge (Edge const &e);

        /// "main" context. As frequently as possible.
        void run ();

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
        static constexpr uint32_t MIN_NOISE_SPIKE_1US = 100;

        // TODO turn off this EXTI as well. EDIT : button would not work. The button pin should be changed to 0-1 then.
        bool isActive () const { return active; }
        bool isBeamClean () const { return beamState == BeamState::present && noiseState == NoiseState::noNoise; }

private:
        StopWatch &stopWatch;

        void reset ()
        {
                highStateStart = middleStateStart = lowStateStart;
                pwmState = PwmState::middle;
        }

        bool active{};
        Result1us minTriggerEvent1Us{};

        /*--------------------------------------------------------------------------*/
        /* Trigger calculations                                                     */
        /*--------------------------------------------------------------------------*/

        EdgeQueue queue;
        IEdgeDetectorCallback *callback{};

        PwmState pwmState;
        Result1us highStateStart{};
        Result1us lowStateStart{};
        Result1us middleStateStart{};

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
