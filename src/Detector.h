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

/**
 * Looks for correct trigger events in the signal.
 */
// class EdgeDetector {
// public:
//         virtual bool onEdge (Edge const &e)
//         {
//                 /*
//                  * This can happen when noise frequency is very high, and the µC can't keep up,
//                  * and its misses an EXTI event. This way we can end up with two consecutive edges
//                  * with the same polarity.
//                  */
//                 if (!queue.empty () && queue.back ().polarity == e.polarity) {
//                         // TODO This is critical situation. Rethink. Maybe report noise?
//                         // queue.clear (); //?
//                         return false;
//                         // report Noise ?

//                         // TODO rethink what to do in this situation. Can this be read from some registers? That we actually missed?
//                         // while (true) {
//                         // }
//                 }

//                 queue.push (e);

//                 if (!queue.full ()) {
//                         return false;
//                 }

//                 if (isTriggerEvent ()) {
//                         callback->report (DetectorEventType::trigger, queue[0].timePoint);
//                         return false;
//                 }

//                 // This is always at the end of the chain, so no next->onEdge
//                 return true;
//         }

//         virtual void run (Result1us const &now) {}

//         void setCallback (IEdgeDetectorCallback *cb) { callback = cb; }

// protected:
//         /// IRQ context
//         bool isTriggerEvent () const
//         {
//                 // Queue should be always full at this point, because there are guards for that.
//                 return queue.front ().polarity == EdgePolarity::rising
//                         && queue[2].timePoint - queue[1].timePoint >= msToResult1 (minTreggerEventMs)
//                         && queue[1].timePoint - queue[0].timePoint >= msToResult1 (minTreggerEventMs);
//         }

//         EdgeQueue queue;
//         IEdgeDetectorCallback *callback{};
// };

/*****************************************************************************/

class EdgeFilter /* : public EdgeDetector */ {
public:
        enum State { low = 0, high = 1 };

        EdgeFilter (/* EdgeDetector *next, */ State initialState) : /* next{next}, */ state{initialState}
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
        // bool longHighState{};

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
