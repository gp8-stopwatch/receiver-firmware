/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef STOPWATCH_H
#define STOPWATCH_H

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
        static constexpr std::array PRESCALERS{10000, 100000, 1000000, 1000000};
        static constexpr std::array PERIODS{100, 100, 100, 10};

        static StopWatch *singleton ()
        {
                static StopWatch st;
                return &st;
        }

        void setResolution (Resolution res);
        void reset () { time = 0; }

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

                // It has value [0, 99]
                if (TIM14->CNT > (PERIODS.at (int (resolution)) / 2) - 1) {
                        // Rounding
                        ++time;
                }
        }

        unsigned int getTime () const { return time; }
        // void setStateMachine (FastStateMachine *s) { this->stateMachine = s; }

        /// Callback gets fired each an every time the time is increased by 1
        // template <typename Fun> void setOnUpdate (Fun &&fun) { onUpdate = std::forward<Fun> (fun); }

        // 100 minutes
        static constexpr unsigned int MAX_TIME = 100U * 60U * 100U - 1U;

private:
        friend void TIM14_IRQHandler ();
        void onInterrupt ();

private:
        // IDisplay *display{};
        TIM_HandleTypeDef stopWatchTimHandle{};
        bool running{};
        unsigned int time{};
        Resolution resolution;
};

#endif // STOPWATCH_H
