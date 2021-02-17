/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Types.h"
#include <cstdint>
#include <etl/circular_buffer.h>

/*****************************************************************************/

// TODO refactor (config?)
const uint16_t minTreggerEventMs = 10;
// TODO config
const uint8_t dutyTresholdPercent = 50;
// TODO config
const uint16_t noiseEventsPerTimeUnit_high = 10;
// TODO config
const uint16_t noiseEventsPerTimeUnit_low = 2;

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
        enum State { low = 0, high = 1 };

        EdgeFilter (State initialState) : state{initialState}
        {
                if (state == State::low) {
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

private:
        Result1us const &getLastStateChange () const { return std::max (highStateStart, lowStateStart); };

        /*--------------------------------------------------------------------------*/
        /* Trigger calculations                                                     */
        /*--------------------------------------------------------------------------*/

        EdgeQueue queue;
        IEdgeDetectorCallback *callback{};

        State state;
        Result1us highStateStart{};
        Result1us lowStateStart{};

        /*--------------------------------------------------------------------------*/
        /* Noise calculations                                                       */
        /*--------------------------------------------------------------------------*/
        enum class NoiseState { noNoise, noise };
        NoiseState noiseState{};
        static constexpr unsigned int NOISE_CALCULATION_PERIOD_MS = 1000;
        int noiseCounter{};
        Result1us lastNoiseCalculation{};

        /*--------------------------------------------------------------------------*/
        /* NoBeam calculations                                                      */
        /*--------------------------------------------------------------------------*/
        enum class BeamState { present, absent };
        BeamState beamState{};
        // int noiseCounter{};
        Result1us lastBeamStateCalculation{};
};
