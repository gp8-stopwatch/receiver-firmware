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
        /*| Stopwatch 32bit main timer (slave)                                      |*/
        /*+-------------------------------------------------------------------------+*/

        mainStopWatchTimHandle.Instance = TIM2;
        mainStopWatchTimHandle.Init.Prescaler = 0;
        mainStopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        mainStopWatchTimHandle.Init.Period = std::numeric_limits<uint32_t>::max ();
        mainStopWatchTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        mainStopWatchTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        __HAL_RCC_TIM2_CLK_ENABLE ();

        if (HAL_TIM_Base_Init (&mainStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        TIM_SlaveConfigTypeDef sSlaveConfig{};
        sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
        sSlaveConfig.InputTrigger = TIM_TS_ITR2;
        sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
        sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;

        if (HAL_TIM_SlaveConfigSynchro (&mainStopWatchTimHandle, &sSlaveConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start (&mainStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Stopwatch 16bit prescaler timer (master)                                |*/
        /*+-------------------------------------------------------------------------+*/

        resolution = res;
        increment = INCREMENTS.at (int (resolution));

        int div = PRESCALERS.at (int (res));
        prescalerStopWatchTimHandle.Instance = TIM3;
        prescalerStopWatchTimHandle.Init.Prescaler = (uint32_t) (HAL_RCC_GetHCLKFreq () / div) - 1;
        prescalerStopWatchTimHandle.Init.Period = PERIODS.at (int (res)) - 1;
        prescalerStopWatchTimHandle.Init.ClockDivision = 0;
        prescalerStopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        prescalerStopWatchTimHandle.Init.RepetitionCounter = 0;

        __HAL_RCC_TIM3_CLK_ENABLE ();

        // Highest priority.
        // HAL_NVIC_SetPriority (TIM3_IRQn, STOPWATCH_PRIORITY, 0);
        // HAL_NVIC_EnableIRQ (TIM3_IRQn);

        if (HAL_TIM_Base_Init (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        TIM_MasterConfigTypeDef sMasterConfig{};
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;

        if (HAL_TIMEx_MasterConfigSynchronization (&prescalerStopWatchTimHandle, &sMasterConfig) != HAL_OK) {
                Error_Handler ();
        }

        // if (HAL_TIM_Base_Start_IT (&prescalerStopWatchTimHandle) != HAL_OK) {
        //         Error_Handler ();
        // }
        if (HAL_TIM_Base_Start (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }
}

/**
 * Stop-watch ISR.
 * Here the value displayed is updated. 100Hz / 1kHz / 10kHz / 100kHz.
 */
// extern "C" void TIM14_IRQHandler ()
// {
//         __HAL_TIM_CLEAR_IT (&StopWatch::singleton ()->prescalerStopWatchTimHandle, TIM_IT_UPDATE);
//         StopWatch::singleton ()->onInterrupt ();
// }

/*****************************************************************************/

inline void StopWatch::onInterrupt ()
{
        if (!running) {
                return;
        }

        time += increment;

        if (time >= MAX_TIME) {
                time = 0;
        }
}
