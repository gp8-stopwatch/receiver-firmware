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

extern "C" void TIM14_IRQHandler ();
class FastStateMachine;
struct IDisplay;

/**
 * Responsible solely for measuring time.
 */
class StopWatch {
public:
        static constexpr std::array PRESCALERS{1000000, 1000000, 100000, 10000};
        static constexpr std::array<uint32_t, 4> PERIODS{10, 100, 100, 100};
        static constexpr uint32_t CAN_LATENCY_CORRECTION = 63;              // Warning! Correct only with -O3
        static constexpr unsigned int MAX_TIME = 100U * 60U * 100000U - 1U; // 100 minutes in 10µs units.
        static constexpr std::array INCREMENTS{1, 10, 100, 1000};

        static StopWatch *singleton ()
        {
                static StopWatch st;
                return &st;
        }

        void setResolution (Resolution res);
        void reset (bool canStart) { time = (canStart) ? CAN_LATENCY_CORRECTION : 0; }

        /**
         * Starts the timer immediatelly.
         */
        void start ()
        {
                running = true;
                TIM14->CNT = 0;
        }

        void stop ()
        {
                running = false;

                // It has value [0, 99] or [0, 9] in case of 10µs resolution
                if (TIM14->CNT >= (PERIODS.at (int (resolution)) / 2)) {
                        // Rounding
                        time += increment;
                }
        }

        unsigned int getTime () const { return time; }

private:
        friend void TIM14_IRQHandler ();
        void onInterrupt ();

        TIM_HandleTypeDef stopWatchTimHandle{};
        bool running{};
        uint32_t time{}; // 10µs incremenets.
        uint32_t increment{};
        Resolution resolution;
};
