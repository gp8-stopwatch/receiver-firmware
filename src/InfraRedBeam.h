/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef INFRAREDBEAM_H
#define INFRAREDBEAM_H

#include <cstdint>
#include <stm32f0xx_hal.h>

// extern "C" void TIM3_IRQHandler ();
// extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler ();

/**
 * How much update events since last rise (noOfUpdateEventsSinceLastRise) indicates
 * that light path is cut. Roughly proportional to ms (???)
 * TODO Is 50 not to much?
 */
#define UPDATE_EVENT_TRESHOLD 20
#define BEAM_GONE 25000

class InfraRedBeam {
public:
        /// If any IR signal rise was observed at all.
        bool isBeamPresent () const { return beamPresent; }

        bool isBeamInterrupted ()
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

        bool isActive () const { return active; }
        void setActive (bool b);
        void reset ();

private:
        TIM_HandleTypeDef timHandle{};
        uint32_t noOfUpdateEventsSinceLastRise = 0;
        uint32_t noOfRises = 0;

        bool beamInterrupted = false;
        bool beamPresent = false;
        bool active = true;

        uint32_t irTime = 0;
        uint32_t timeOfLastRise = 0;
        uint32_t noOfSuccesiveRises = 0;
};

#endif // INFRAREDBEAM_H
