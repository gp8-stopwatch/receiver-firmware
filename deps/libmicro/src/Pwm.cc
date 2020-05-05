/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Pwm.h"
#include "ErrorHandler.h"
#include <cstring>

Pwm::Pwm (TIM_TypeDef *instance, uint32_t prescaler, uint32_t period)
{
        memset (&htim, 0, sizeof (htim));
        htim.Instance = instance;
        htim.Init.Prescaler = prescaler;
        htim.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim.Init.Period = period;
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim.Init.RepetitionCounter = 0;

        clkEnable ();

        if (HAL_TIM_PWM_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        //        TIM_OC_InitTypeDef sConfigOC;
        //        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        //        sConfigOC.Pulse = 0;
        //        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        //        sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        //        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        //        sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
        //        sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        //        if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        //                Error_Handler ();
        //        }

        //        HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_1);

        //        if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        //                Error_Handler ();
        //        }

        //        HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_2);

        //        if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
        //                Error_Handler ();
        //        }

        //        HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_3);

        //        if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
        //                Error_Handler ();
        //        }

        //        HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_4);

        //        TIM1->CCR1 = 500;
        //        TIM1->CCR2 = 0;
        //        TIM1->CCR3 = 0;
        //        TIM1->CCR4 = 1000;
}

/*****************************************************************************/

void Pwm::clkEnable (TIM_HandleTypeDef *timer)
{
        if (timer->Instance == TIM1) {
                __HAL_RCC_TIM1_CLK_ENABLE ();
        }
        else if (timer->Instance == TIM2) {
                __HAL_RCC_TIM2_CLK_ENABLE ();
        }
        else if (timer->Instance == TIM3) {
                __HAL_RCC_TIM3_CLK_ENABLE ();
        }
// TODO Clen up those stupid macros
#ifdef LIB_MICRO_STM32F0
        // TODO cover all timers!!!
        else if (timer->Instance == TIM17) {
                __HAL_RCC_TIM17_CLK_ENABLE ();
        }
#endif
}

/*****************************************************************************/

void Pwm::clkDisable (TIM_HandleTypeDef *timer)
{
        if (timer->Instance == TIM1) {
                __HAL_RCC_TIM1_CLK_DISABLE ();
        }
        else if (timer->Instance == TIM2) {
                __HAL_RCC_TIM2_CLK_DISABLE ();
        }
        else if (timer->Instance == TIM3) {
                __HAL_RCC_TIM3_CLK_DISABLE ();
        }
// TODO Clen up those stupid macros
#ifdef LIB_MICRO_STM32F0
        // TODO cover all timers!!!
        else if (timer->Instance == TIM17) {
                __HAL_RCC_TIM17_CLK_DISABLE ();
        }
#endif
}

/*****************************************************************************/

void Pwm::enableChannels (uint8_t channels)
{
        TIM_OC_InitTypeDef sConfigOC;
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
        sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        if (channels & CHANNEL1) {
                if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_1);
        }

        if (channels & CHANNEL2) {
                if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_2);
        }

        if (channels & CHANNEL3) {
                if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_3);
        }

        if (channels & CHANNEL4) {
                if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_4);
        }
}

/*****************************************************************************/

void Pwm::setDuty (uint8_t channels, uint32_t duty)
{
        if (channels & CHANNEL1) {
                htim.Instance->CCR1 = duty;
        }

        if (channels & CHANNEL2) {
                htim.Instance->CCR2 = duty;
        }

        if (channels & CHANNEL3) {
                htim.Instance->CCR3 = duty;
        }

        if (channels & CHANNEL4) {
                htim.Instance->CCR4 = duty;
        }
}
