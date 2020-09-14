/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "InfraRedBeamModulated.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"

/*****************************************************************************/

// void InfraRedBeamModulated::init ()
//{

//        /*+-------------------------------------------------------------------------+*/
//        /*| IR detection timer 10kHz                                                |*/
//        /*+-------------------------------------------------------------------------+*/
//        timHandle.Instance = TIM1;

//        // 10kHz
//        timHandle.Init.Period = (uint32_t) ((HAL_RCC_GetHCLKFreq () / 2) / 10000) - 1; // 8399
//        timHandle.Init.Prescaler = 0;

//        // timHandle.Init.Prescaler = (uint32_t)((HAL_RCC_GetHCLKFreq () / 2) / 840000) - 1; // 1 tick = 84MHz/100 = 840kHz
//        // timHandle.Init.Period = 8400 - 1;                                                 // Update event every 10ms = 100Hz
//        timHandle.Init.ClockDivision = 0;
//        timHandle.Init.CounterMode = TIM_COUNTERMODE_UP;

//        __HAL_RCC_TIM1_CLK_ENABLE ();
//        HAL_NVIC_SetPriority (TIM1_BRK_UP_TRG_COM_IRQn, 3, 0);
//        HAL_NVIC_EnableIRQ (TIM1_BRK_UP_TRG_COM_IRQn);

//        if (HAL_TIM_Base_Init (&timHandle) != HAL_OK) {
//                Error_Handler ();
//        }

//        if (HAL_TIM_Base_Start_IT (&timHandle) != HAL_OK) {
//                Error_Handler ();
//        }

//        // Konfigureacja kanału. Input Capture. Wejście.
//        __HAL_RCC_GPIOA_CLK_ENABLE ();
//        GPIO_InitTypeDef gpioInitStruct;
//        gpioInitStruct.Pin = GPIO_PIN_8;
//        gpioInitStruct.Mode = GPIO_MODE_AF_PP;
//        gpioInitStruct.Pull = GPIO_PULLDOWN;
//        gpioInitStruct.Alternate = GPIO_AF2_TIM1;
//        gpioInitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//        HAL_GPIO_Init (GPIOA, &gpioInitStruct);

//        // Dotycząca timera
//        TIM_IC_InitTypeDef sICConfig;
//        sICConfig.ICPolarity = TIM_ICPOLARITY_RISING;
//        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
//        sICConfig.ICFilter = 0;
//        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;

//        if (HAL_TIM_IC_ConfigChannel (&timHandle, &sICConfig, TIM_CHANNEL_1) != HAL_OK) {
//                Error_Handler ();
//        }

//        if (HAL_TIM_IC_Start_IT (&timHandle, TIM_CHANNEL_1) != HAL_OK) {
//                Error_Handler ();
//        }
//}

/*****************************************************************************/

// extern "C" void TIM3_IRQHandler () { InfraRedBeamModulated::singleton ()->onInterrupt (); }
// extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler () { InfraRedBeamModulated::singleton ()->onInterrupt (); }

/*****************************************************************************/

// #if 0
// void InfraRedBeamModulated::run ()
// {
//         static uint32_t time = 0;
//         static uint32_t timeOfLastRise = 0;
//         static uint32_t noOfSuccesiveRises = 0;

//         /*
//          * I.C. ~1kHz
//          * Uwaga! Makro __HAL_TIM_GET_IT_SOURCE ma mylną nazwę, bo ono sprawdza rejestr DIER, czyli
//          * sprawdza, czy dane przerwanie jest WŁĄCZONE czy nie. Jeśli by nie było włączone, to byśmy
//          * nigdy się nie znaleźli w ISR z powodu tego konkretnego przerwania.
//          */
//         if (__HAL_TIM_GET_FLAG (&timHandle, TIM_FLAG_CC1) /*&& __HAL_TIM_GET_IT_SOURCE (&timHandle, TIM_IT_CC1)*/) {
//                 __HAL_TIM_CLEAR_IT (&timHandle, TIM_IT_CC1);

//                 timeOfLastRise = time;

//                 if (time - timeOfLastRise <= 12) {
//                         if (++noOfSuccesiveRises >= 10) {
//                                 beamPresent = true;
//                         }
//                 }

//                 return;
//         }

//         // 10kHz
//         if (__HAL_TIM_GET_FLAG (&timHandle, TIM_FLAG_UPDATE) && __HAL_TIM_GET_IT_SOURCE (&timHandle, TIM_IT_UPDATE)) {
//                 __HAL_TIM_CLEAR_IT (&timHandle, TIM_IT_UPDATE);

//                 // Will re-wind to 0 after ~120hours
//                 ++time;

//                 // Znikło na 100ms
//                 if (time - timeOfLastRise > 1000) {
//                         beamPresent = false;
//                         noOfSuccesiveRises = 0;
//                 }

//                 // Znikło na 7ms
//                 if (time - timeOfLastRise > 70) {
//                         beamInterrupted = true;
//                 }
//         }
// }
// #endif

void InfraRedBeamModulated::on10kHz ()
{
        if (!active) {
                return;
        }

        // Will re-wind to 0 after ~120hours
        ++irTime;

        // Znikło na 100ms
        if (irTime - timeOfLastRise > 1000) {
                beamPresent = false;
                noOfSuccesiveRises = 0;
        }

        // Znikło na 7ms, ale wcześniej było
        if (irTime - timeOfLastRise > 70 && beamPresent) {
                beamInterrupted = true;
        }
}

/*****************************************************************************/

void InfraRedBeamModulated::on1kHz ()
{
        if (!active) {
                return;
        }

        timeOfLastRise = irTime;

        if (irTime - timeOfLastRise <= 12) {
                if (++noOfSuccesiveRises >= 10) {
                        beamPresent = true;
                }
        }
}

/*****************************************************************************/

void InfraRedBeamModulated::setActive (bool b)
{
        active = b;

        if (!b) {
                reset ();
        }
}

/*****************************************************************************/

void InfraRedBeamModulated::reset ()
{
        noOfUpdateEventsSinceLastRise = 0;
        noOfRises = 0;

        beamInterrupted = false;
        beamPresent = false;

        irTime = 0;
        timeOfLastRise = 0;
        noOfSuccesiveRises = 0;
}
