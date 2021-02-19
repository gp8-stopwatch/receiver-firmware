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
using EdgeQueue = etl::circular_buffer<Edge, 3>;

enum class DetectorEventType { trigger, noise, noNoise, noBeam, beamRestored };

/**
 *
 */
struct IEdgeDetectorCallback {
        virtual void report (DetectorEventType type, Result1us timePoint) = 0;
};

/*****************************************************************************/

class EdgeFilter {
public:
        enum class PwmState { low = 0, high = 1 };
        enum class TriggerLevelState { idle, high, /*  low */ };

        EdgeFilter (PwmState initialState) : pwmState{initialState}
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
                }
        }

        /// IRQ context
        void onEdge (Edge const &e);

        /// "main" context. As frequently as possible.
        void run (Result1us const &now);

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
        static constexpr uint32_t NO_BEAM_CALCULATION_PERIOD_MS = 1000;

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

private:
        Result1us const &getLastStateChange () const { return std::max (highStateStart, lowStateStart); };

        Result1us minTriggerEvent1Us{};

        /*--------------------------------------------------------------------------*/
        /* Trigger calculations                                                     */
        /*--------------------------------------------------------------------------*/

        EdgeQueue queue;
        IEdgeDetectorCallback *callback{};

        PwmState pwmState;
        TriggerLevelState triggerLevelState{TriggerLevelState::idle};
        Result1us highStateStart{};
        Result1us lowStateStart{};

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
        Result1us highStateAveragePeriod{};
};
