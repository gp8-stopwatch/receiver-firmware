/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Timer.h"
#include <cstdint>
#include <functional>
#include <stm32f0xx_hal.h>

class FastStateMachine;
struct IDisplay;

/**
 * Responsible solely for measuring time.
 */
class StopWatch {
public:
        StopWatch ();

        /// Overflows every 2^48µs which is 8,9 years. No protection against this.
        Result1us getTimeFromIsr () const
        {
                Result1us now = TIM3->CNT;
                now <<= 32;
                now |= TIM2->CNT;
                return now;
        }

        Result1us getTime () const
        {
                __disable_irq ();
                auto res = getTimeFromIsr ();
                __enable_irq ();
                return res;
        }

private:
        TIM_HandleTypeDef prescalerStopWatchTimHandle{};
        TIM_HandleTypeDef mainStopWatchTimHandle{};
};
