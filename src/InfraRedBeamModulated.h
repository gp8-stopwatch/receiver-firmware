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

class FastStateMachine;

/**
 * How much update events since last rise (noOfUpdateEventsSinceLastRise) indicates
 * that light path is cut. Roughly proportional to ms (???)
 * TODO Is 50 not to much?
 */
#define UPDATE_EVENT_TRESHOLD 20
// #define BEAM_GONE 25000

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
