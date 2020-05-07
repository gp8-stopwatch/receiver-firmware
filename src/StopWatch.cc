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

StopWatch::StopWatch () : running (false), time (0) {}

/*****************************************************************************/

void StopWatch::setResolution (Resolution res)
{
        /*+-------------------------------------------------------------------------+*/
        /*| Stopwatch timer 100Hz                                                   |*/
        /*+-------------------------------------------------------------------------+*/

        int div = 10000;

        switch (res) {
        case Resolution::ms_10:
                div = 10000; // 100Hz
                break;

        case Resolution::ms_1:
                div = 100000; // 1kHz
                break;

        case Resolution::ms_01:
                div = 1000000; // 10kHz
                break;

        default:
                break;
        }

        stopWatchTimHandle.Instance = TIM14;
        stopWatchTimHandle.Init.Prescaler = (uint32_t) (HAL_RCC_GetHCLKFreq () / div) - 1;
        stopWatchTimHandle.Init.Period = 100 - 1;
        stopWatchTimHandle.Init.ClockDivision = 0;
        stopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        stopWatchTimHandle.Init.RepetitionCounter = 0;

        // Uwaga! Zapisać to!!! Msp init jest wywoływane PRZED TIM_Base_SetConfig
        __HAL_RCC_TIM14_CLK_ENABLE ();
        HAL_NVIC_SetPriority (TIM14_IRQn, 1, 0);
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
 * Here the value displayed is updated. 100Hz
 */
extern "C" void TIM14_IRQHandler ()
{
        __HAL_TIM_CLEAR_IT (&StopWatch::singleton ()->stopWatchTimHandle, TIM_IT_UPDATE);
        StopWatch::singleton ()->onInterrupt ();
}

/*****************************************************************************/

void StopWatch::onInterrupt ()
{
        stateMachine->run ();

        if (!running) {
                return;
        }

        if (++time >= MAX_TIME) {
                time = 0;
        }

        display->setTime (time);
}
