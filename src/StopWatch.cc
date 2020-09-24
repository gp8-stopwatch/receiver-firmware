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

StopWatch::StopWatch ()
{
        /*+-------------------------------------------------------------------------+*/
        /*| Stopwatch 32bit main timer (slave) counting 100kHz from TIM3            |*/
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
        /*| Stopwatch 16bit prescaler timer (master) 100kHz                         |*/
        /*+-------------------------------------------------------------------------+*/

        // int div = PRESCALERS.at (int (res));
        prescalerStopWatchTimHandle.Instance = TIM3;
        prescalerStopWatchTimHandle.Init.Prescaler = (uint32_t) (HAL_RCC_GetHCLKFreq () / 1000000) - 1;
        prescalerStopWatchTimHandle.Init.Period = 10 - 1;
        prescalerStopWatchTimHandle.Init.ClockDivision = 0;
        prescalerStopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        prescalerStopWatchTimHandle.Init.RepetitionCounter = 0;

        __HAL_RCC_TIM3_CLK_ENABLE ();

        if (HAL_TIM_Base_Init (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        TIM_MasterConfigTypeDef sMasterConfig{};
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;

        if (HAL_TIMEx_MasterConfigSynchronization (&prescalerStopWatchTimHandle, &sMasterConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }
}
