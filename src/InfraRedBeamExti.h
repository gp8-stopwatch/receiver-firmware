/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Gpio.h"
#include "Hal.h"
#include "IInfraRedBeam.h"
#include "Timer.h"
#include "Types.h"
#include <cstdint>
#include <stm32f0xx_hal.h>

class FastStateMachine;
class StopWatch;
class Gpio;

/**
 * Non modulatred for IR barriers and curtains like TSSP 4056.
 * They output
 */
class InfraRedBeamExti : public IInfraRedBeam {
public:
        static constexpr int MIN_TIME_BETWEEN_EVENTS_MS = 10;
        static constexpr int NOISE_CLEAR_TIMEOUT_MS = 1000;
        static constexpr int NOISE_EVENTS_CRITICAL = 5;

        /// Based on what was the state at the time of powering on.
        // explicit InfraRedBeamExti (IrBeam initialState) : lastState{initialState} {}
        explicit InfraRedBeamExti (Gpio &g) : irTriggerPin{g}, lastState{getPinState ()} {}

        void onExti ();

        /// Returns false if the beam was interrupted for more than 3s. True otherwise.
        IrBeam getBeamState () const override
        {
                // Noise is always returned immediately
                if (lastState == IrBeam::noise) {
                        return IrBeam::noise;
                }

                if (beamPresentTimer.isExpired ()) {
                        return lastState;
                }

                return IrBeam::present;
        }

        /// This metod does not make sense when using EXTI (this method is for polling)
        bool isBeamInterrupted () const override { return false; }

        bool isActive () const override { return active; }
        void setActive (bool b) override { active = b; }

        void setFastStateMachine (FastStateMachine *f) { fStateMachine = f; }
        void run ();

        void setStopWatch (StopWatch *s) { this->stopWatch = s; }

private:
        // IR pin level, converted to IrState. No logic.
        IrBeam getPinState () const { return (irTriggerPin.get ()) ? (IrBeam::absent) : (IrBeam::present); }

        Timer beamPresentTimer;
        Timer beamNoiseTimer{NOISE_CLEAR_TIMEOUT_MS};
        Timer eventValidationTimer;
        int noiseEventCounter{};
        Gpio &irTriggerPin;
        IrBeam lastState;
        bool active{true};
        FastStateMachine *fStateMachine{};
        StopWatch *stopWatch{};
        Result triggerRisingEdge{};
};