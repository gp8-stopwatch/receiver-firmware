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

        /**
         * The critical section here (__disable_irq) is to be considered:
         * - If it's present, there is a chance that some EXTI IRQ will be postponed.
         * - If it's not present, there is a chance that tis method would be preempted,
         * and variables corrupted. But this will only happen in the main thread, while
         * in the EXTI IRQ this method won't get preempted, as EXTI has the highest priority.
         */
        Result1us getTime () const
        {
                // __disable_irq ();

                // This value is 32 bit, so this is why I introducet epochs.
                Result1us now = TIM2->CNT * 10LL + TIM3->CNT;

                // When TIM2 overflows, I increase the epoch and thus I have 64 bit value.
                if (now < last) {
                        ++timerEpoch;
                }

                last = now;
                auto ret = now + timerEpoch * (Result1us (std::numeric_limits<uint32_t>::max ()) + 1) * 10LL;
                // __enable_irq ();
                return ret;
        }

        void run ()
        {
                if (runTimer.isExpired ()) {
                        (void)getTime ();
                        runTimer.start (RUN_INTERVAL_MS);
                }
        }

private:
        TIM_HandleTypeDef prescalerStopWatchTimHandle{};
        TIM_HandleTypeDef mainStopWatchTimHandle{};
        mutable Result1us last{};
        mutable uint16_t timerEpoch{};

        static constexpr uint32_t RUN_INTERVAL_MS = 1000;
        Timer runTimer{};
};
