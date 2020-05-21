/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "StopWatch.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"
#include "IDisplay.h"
#include <array>

/****************************************************************************/

void StopWatch::setResolution (Resolution res)
{
        if (running) {
                std::terminate ();
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Stopwatch timer 100Hz                                                   |*/
        /*+-------------------------------------------------------------------------+*/

        static constexpr std::array PRESCALERS{10000, 100000, 1000000};
        int div = PRESCALERS.at (int (res));

        stopWatchTimHandle.Instance = TIM14;
        stopWatchTimHandle.Init.Prescaler = (uint32_t) (HAL_RCC_GetHCLKFreq () / div) - 1;
        stopWatchTimHandle.Init.Period = 100 - 1;
        stopWatchTimHandle.Init.ClockDivision = 0;
        stopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        stopWatchTimHandle.Init.RepetitionCounter = 0;

        __HAL_RCC_TIM14_CLK_ENABLE ();

        // Highest priority.
        HAL_NVIC_SetPriority (TIM14_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ (TIM14_IRQn);

        if (HAL_TIM_Base_Init (&stopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start_IT (&stopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }
}

/**
 * Stop-watch ISR.
 * Here the value displayed is updated. 100Hz / 1kHz / 10kHz.
 */
extern "C" void TIM14_IRQHandler ()
{
        __HAL_TIM_CLEAR_IT (&StopWatch::singleton ()->stopWatchTimHandle, TIM_IT_UPDATE);
        StopWatch::singleton ()->onInterrupt ();
}

/*****************************************************************************/

void StopWatch::onInterrupt ()
{
        // if (onUpdate) {
        //         onUpdate ();
        // }

        if (!running) {
                return;
        }

        if (++time >= MAX_TIME) {
                time = 0;
        }
}