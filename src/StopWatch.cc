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
        /*| TIM2 : 32bit @ 1MHz                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        mainStopWatchTimHandle.Instance = TIM2;
        mainStopWatchTimHandle.Init.Prescaler = (uint32_t) (HAL_RCC_GetHCLKFreq () / 1000000) - 1; // 1MHz
        mainStopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        mainStopWatchTimHandle.Init.Period = std::numeric_limits<uint32_t>::max ();
        mainStopWatchTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        mainStopWatchTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        __HAL_RCC_TIM2_CLK_ENABLE ();

        if (HAL_TIM_Base_Init (&mainStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        TIM_MasterConfigTypeDef sMasterConfig{};
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;

        if (HAL_TIMEx_MasterConfigSynchronization (&mainStopWatchTimHandle, &sMasterConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start (&mainStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        /*+-------------------------------------------------------------------------+*/
        /*| TIM3 : 16bit @ every TIM2 update event (~4000s)                         |*/
        /*+-------------------------------------------------------------------------+*/

        prescalerStopWatchTimHandle.Instance = TIM3;
        prescalerStopWatchTimHandle.Init.Prescaler = 0;
        prescalerStopWatchTimHandle.Init.Period = std::numeric_limits<uint16_t>::max ();
        prescalerStopWatchTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        prescalerStopWatchTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        prescalerStopWatchTimHandle.Init.RepetitionCounter = 0;

        __HAL_RCC_TIM3_CLK_ENABLE ();

        if (HAL_TIM_Base_Init (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        TIM_SlaveConfigTypeDef sSlaveConfig{};
        sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
        sSlaveConfig.InputTrigger = TIM_TS_ITR1; // Table 66. TIM2 and TIM3 internal trigger connection. RM page 440.
        sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
        sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;

        if (HAL_TIM_SlaveConfigSynchro (&prescalerStopWatchTimHandle, &sSlaveConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start (&prescalerStopWatchTimHandle) != HAL_OK) {
                Error_Handler ();
        }

        // This is for testing TIM2 overflow
        // TIM2->CNT = std::numeric_limits<uint32_t>::max () - 10 * 100000;
        TIM2->CNT = 0;
        TIM3->CNT = 0;
}
