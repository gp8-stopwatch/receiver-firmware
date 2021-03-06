/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Detector.h"
#include "Types.h"
#include <cstdint>
#ifndef UNIT_TEST
#include "StopWatch.h"
#endif

/**
 * Responsible for reacting to the IR sesnor output as fast as possible, minimizing
 * noise impact on the accuracy of the measuremetns, detecting excessive noise levels,
 * detecting IR signal state.
 */
class ExtTriggerDetector {
public:
#ifndef UNIT_TEST
        ExtTriggerDetector (PwmState initialState, StopWatch &st)
            : stopWatch{st}, queue{(initialState == PwmState::low) ? (EdgePolarity::rising) : (EdgePolarity::falling)}, pwmState
        {
                initialState
        }
#else
        ExtTriggerDetector (PwmState initialState)
            : queue{(initialState == PwmState::low) ? (EdgePolarity::rising) : (EdgePolarity::falling)}, pwmState
        {
                initialState
        }
#endif
        {
        }

        /// IRQ context
        void onEdge (Edge const &e, EdgePolarity pol);

        /// non-IRQ
        void onSynchronization (Result1usLS time) { synchronizationDelay = lsResult (time); }

        /// "main" context. As frequently as possible.
#ifndef UNIT_TEST
        void run ();
#else
        void run (Result1us now);
#endif

        void setCallback (IEdgeDetectorCallback *cb) { callback = cb; }
        bool isBeamClean () const { return noiseState == NoiseState::noNoise; }

#ifndef UNIT_TEST
private:
        StopWatch &stopWatch;
#endif

        void reset ()
        {
                highStateStart = lowStateStart;
                pwmState = PwmState::middle;
        }

        /// Final check if we have proper trigger event
        void checkForEventCondition (Edge const &e);

        /*--------------------------------------------------------------------------*/
        /* Trigger calculations                                                     */
        /*--------------------------------------------------------------------------*/

        EdgeQueue queue;
        IEdgeDetectorCallback *callback{};

        PwmState pwmState;
        Result1us highStateStart{};
        Result1us lowStateStart{};

        Result1us synchronizationDelay{};

        /*--------------------------------------------------------------------------*/
        /* Noise calculations                                                       */
        /*--------------------------------------------------------------------------*/
        enum class NoiseState { noNoise, testing, noise };
        NoiseState noiseState{};
        int noiseCounter{};
        Result1us lastNoiseCalculation{};
        void noiseDetection (Result1us now);
};
