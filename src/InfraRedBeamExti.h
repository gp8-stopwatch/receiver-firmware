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

/**
 * How much update events since last rise (noOfUpdateEventsSinceLastRise) indicates
 * that light path is cut. Roughly proportional to ms (???)
 * TODO Is 50 not to much?
 */
#define UPDATE_EVENT_TRESHOLD 20
// #define BEAM_GONE 25000

/**
 * Non modulatred for IR barriers and curtains like TSSP 4056.
 * They output
 */
class InfraRedBeamExti : public IInfraRedBeam {
public:
        /// Based on what was the state at the time of powering on.
        explicit InfraRedBeamExti (IrBeam initialState) : lastState{initialState} {}

        void onExti (IrBeam state);

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

private:
        Timer beamPresentTimer;
        Timer beamNoiseTimer;
        int noiseEventCounter{};
        IrBeam lastState;
        bool active{true};
        FastStateMachine *fStateMachine{};
};
