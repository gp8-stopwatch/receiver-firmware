/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "BlindManager.h"
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
class IrTriggerDetector {
public:
#ifndef UNIT_TEST
        IrTriggerDetector (PwmState initialState, StopWatch &st)
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
        void onEdge (Edge const &e, EdgePolarity pol);

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
        NoiseState getNoiseState () const { return noiseState; }

        /*
         * Most severe (the shortest) noise spike we anticipate for.
         * This is not to qualify a signal change as a noise spike or not. This is
         * to compare noise events to the most severe (the shortest) transient signal
         * change we anticipate for. This is only for calculating the noise level.
         *
         * This value depends mostly (or solely) on the IR receiver used and its bandwith
         * (at least this is my understanding).
         */
        static constexpr int MIN_NOISE_SPIKE_1US = 100;
        static constexpr uint32_t NOISE_CALCULATION_PERIOD_US = static_cast<uint32_t> (resultLS (msToResult1us (NOISE_CALCULATION_PERIOD_MS)));
        static constexpr uint32_t MAX_NOISE_EVENTS_NUMBER_PER_PERIOD = 150; // Empoirical for trigger 10 ms

        // TODO turn off this EXTI as well.
        bool isActive () const { return active; }
        bool isBeamClean () const { return beamState == BeamState::present && noiseState == NoiseState::noNoise; }
        BeamState getBeamState () const { return beamState; }

        void setBlindManager (BlindManager *b) { blindManager = b; }

#ifndef UNIT_TEST
private:
        StopWatch &stopWatch;
#endif

        void reset ()
        {
                highStateStart = /* middleStateStart =  */ lowStateStart;
                pwmState = PwmState::middle; // TODO ?
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

        /*--------------------------------------------------------------------------*/
        /* Noise calculations                                                       */
        /*--------------------------------------------------------------------------*/
        NoiseState noiseState{};
        int noiseCounter{};
        Result1us lastNoiseCalculation{};
        uint8_t noiseLevel{};

        /*--------------------------------------------------------------------------*/
        /* NoBeam calculations                                                      */
        /*--------------------------------------------------------------------------*/
        BeamState beamState{};
        Result1us lastBeamStateCalculation{};

        /*--------------------------------------------------------------------------*/
        /* Blind period calculations                                                */
        /*--------------------------------------------------------------------------*/
        BlindManager *blindManager{};

        /*--------------------------------------------------------------------------*/
        /* External trigger management.                                             */
        /*--------------------------------------------------------------------------*/
        enum class ExtTriggerState { low, high };
        ExtTriggerState triggerState{};
        Result1usLS highTriggerStateStart{};
        void extTriggerToHigh (Result1usLS now);
        void extTriggerToLowIf (Result1usLS now);
};
