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
        // static constexpr std::array PRESCALERS{1000000, 1000000, 100000, 10000};
        // static constexpr std::array<uint32_t, 4> PERIODS{10, 100, 100, 100};
        // Warning! Correct only with -O3. For a CanFrame with DLC == 0 63 was OK. This is for DLC == 4

        // TODO - why this is not used!?!?!?
        static constexpr Result CAN_LATENCY_CORRECTION = 98;

        // static constexpr unsigned int MAX_TIME = 100U * 60U * 100000U - 1U; // 100 minutes in 10µs units.
        // static constexpr std::array INCREMENTS{1, 10, 100, 1000};

        static StopWatch *singleton ()
        {
                static StopWatch st;
                return &st;
        }

        StopWatch ();

        // void setResolution (Resolution res) { resolution = res; }
        // void reset (bool canStart)
        // { /* time = (canStart) ? (CAN_LATENCY_CORRECTION) : (0); */
        // }

        void set (Result actualTime)
        {
                TIM3->CNT = 0;
                TIM2->CNT = actualTime;
        }

        void substract (Result actualTime)
        {
                // TIM3->CNT = 0;
                auto tmp = TIM2->CNT;
                tmp -= actualTime;
                TIM2->CNT = tmp;
        }

        /**
         * Starts the timer immediatelly.
         */
        // void start ()
        // {
        //         // running = true;
        //         TIM3->CNT = 0;
        //         TIM2->CNT = 0;

        //         HAL_TIM_Base_Start (&mainStopWatchTimHandle);
        //         HAL_TIM_Base_Start (&prescalerStopWatchTimHandle);
        // }

        // void stop ()
        // {
        //         HAL_TIM_Base_Stop (&prescalerStopWatchTimHandle);
        //         HAL_TIM_Base_Stop (&mainStopWatchTimHandle);
        // }

        Result getTime () const
        {
                auto t = TIM2->CNT;

                if (TIM3->CNT >= (10 / 2)) {
                        ++t;
                }

                return t;
        }

private:
        TIM_HandleTypeDef prescalerStopWatchTimHandle{};
        TIM_HandleTypeDef mainStopWatchTimHandle{};
};
