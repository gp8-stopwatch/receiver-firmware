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
 * Non modulated for IR barriers and curtains like TSSP 4056.
 * They output
 */
class InfraRedBeamExti : public IInfraRedBeam {
public:
        static constexpr int MIN_TIME_BETWEEN_EVENTS_MS = 10;
        static constexpr int MIN_TRIGGER_LOW_STEADY_TIME_MS = 10;
        static constexpr int NOISE_CLEAR_TIMEOUT_MS = 1000;
        static constexpr int NOISE_EVENTS_CRITICAL = 100;
        static constexpr float MIN_DUTY_CYCLE = 0.5F;

        enum class State { idle, irTriggerLow, irTriggerHigh, extTriggerLow, extTriggerHigh, noise, noBeam };

        /// Based on what was the state at the time of powering on.
        // explicit InfraRedBeamExti (IrBeam initialState) : lastState{initialState} {}
        InfraRedBeamExti (Gpio &irTriggerPin, Gpio &extTriggerOutput, Gpio &extTriggerOutEnable)
            : irTriggerPin{irTriggerPin}, extTriggerOutput{extTriggerOutput}, extTriggerOutEnable{extTriggerOutEnable}, lastState{getPinState ()}
        {
        }

        void onExti (IrBeam state, bool external);
        void onExti2 (IrBeam state, bool external);
        void run ();
        void run2 ();

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

                return IrBeam::triggerFalling;
        }

        /// This metod does not make sense when using EXTI (this method is for polling)
        bool isBeamInterrupted () const override { return false; }

        bool isActive () const override { return active; }
        void setActive (bool b) override { active = b; }

        void setFastStateMachine (FastStateMachine *f) { fStateMachine = f; }
        void setStopWatch (StopWatch *s) { this->stopWatch = s; }

        // IR pin level, converted to IrState. No logic.
        IrBeam getPinState () const { return (irTriggerPin.get ()) ? (IrBeam::triggerRising) : (IrBeam::triggerFalling); }

private:
        Timer beamPresentTimer;
        Timer beamNoiseTimer{NOISE_CLEAR_TIMEOUT_MS};
        Timer blindTimeout;
        Timer refreshTimer;

        /*
         * How often to check if trigger was captured (measurements are precise,
         * only this check is done less frequently because IRQs are disabled during
         * this period).
         */
        static constexpr int REFRESH_TIMEOUT_MS = 100;

        std::optional<Result1us> triggerRisingEdgeTime{};
        Result1us triggerFallingEdgeTime{};
        Result1us lastIrChangeTimePoint{};

        Result1us irPresentPeriod{};
        Result1us irAbsentPeriod{};

        Result1us triggerHighPeriod{};
        Result1us triggerLowPeriod{};

        // int dutyCycle{}; // Actual dutyCycle in percent.

        State state{};

        int noiseEventCounter{};
        Gpio &irTriggerPin;
        Gpio &extTriggerOutput;
        Gpio &extTriggerOutEnable;
        IrBeam lastState{};
        bool active{true};
        FastStateMachine *fStateMachine{};
        StopWatch *stopWatch{};
};
