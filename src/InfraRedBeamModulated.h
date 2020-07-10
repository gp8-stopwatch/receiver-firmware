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
#include "Timer.h"
#include "Types.h"
#include <cstdint>
#include <stm32f0xx_hal.h>

/**
 * How much update events since last rise (noOfUpdateEventsSinceLastRise) indicates
 * that light path is cut. Roughly proportional to ms (???)
 * TODO Is 50 not to much?
 */
#define UPDATE_EVENT_TRESHOLD 20
// #define BEAM_GONE 25000

/**
 *Base for various receivers.
 */
struct IInfraRedBeam {
        virtual bool isBeamPresent () const = 0;
        virtual bool isBeamInterrupted () const = 0;

        virtual bool isActive () const = 0;
        virtual void setActive (bool b) = 0;
};

/**
 * form modulated devices like TSOP 38338
 */
class InfraRedBeamModulated : public IInfraRedBeam {
public:
        /// If any IR signal rise was observed at all.
        bool isBeamPresent () const override { return beamPresent; }

        bool isBeamInterrupted () const override
        {
                if (beamInterrupted) {
                        beamInterrupted = false;
                        return true;
                }

                return false;
        }

        // To be in 10kHz
        // void run ();

        /// Call from a timer
        void on10kHz ();

        /// Call from a timer
        void on1kHz ();

        bool isActive () const override { return active; }
        void setActive (bool b) override;
        void reset ();

private:
        TIM_HandleTypeDef timHandle{};
        uint32_t noOfUpdateEventsSinceLastRise = 0;
        uint32_t noOfRises = 0;

        mutable bool beamInterrupted = false;
        bool beamPresent = false;
        bool active = true;

        uint32_t irTime = 0;
        uint32_t timeOfLastRise = 0;
        uint32_t noOfSuccesiveRises = 0;
};

/// State of the IR beam (light).
enum class IrBeam { absent, present };

/**
 * Non modulatred for IR barriers and curtains like TSSP 4056.
 * They output
 */
class InfraRedBeamExti : public IInfraRedBeam {
public:
        /// Based on what was the state at the time of powering on.
        explicit InfraRedBeamExti (IrBeam initialState) : lastState{initialState} {}

        void onExti (IrBeam state)
        {
                if (!active) {
                        return;
                }

                if (state == IrBeam::absent) {
                        onTrigger ();
                        lastStateChange.start (BEAM_INTERRUPTION_EVENT);
                }
                else {
                        lastStateChange.start (0);
                }

                lastState = state;
        }

        /// Returns false if the beam was interrupted for more than 3s. True otherwise.
        bool isBeamPresent () const override
        {
                if (lastStateChange.isExpired ()) {
                        return lastState == IrBeam::present;
                }

                return true;
        }

        /// This metod does not make sense when using EXTI (this method is for polling)
        bool isBeamInterrupted () const override { return false; }

        bool isActive () const override { return active; }
        void setActive (bool b) override
        {
                active = b;
                // lastState = IrBeam::absent;
                // lastStateChange.start (BEAM_INTERRUPTION_EVENT);
        }

        // TODO do not use dynamic allocation.
        std::function<void ()> onTrigger;

private:
        Timer lastStateChange;
        IrBeam lastState;
        bool active{true};
};
