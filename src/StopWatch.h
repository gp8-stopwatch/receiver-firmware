/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
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
        static StopWatch *singleton ()
        {
                static StopWatch st;
                return &st;
        }

        StopWatch ();

        Result1us getTime () const
        {
                // auto t = TIM2->CNT;

                // if (TIM3->CNT >= (10 / 2)) {
                //         ++t;
                // }

                // return t;
                return TIM2->CNT * 10 + TIM3->CNT;
        }

private:
        TIM_HandleTypeDef prescalerStopWatchTimHandle{};
        TIM_HandleTypeDef mainStopWatchTimHandle{};
};
