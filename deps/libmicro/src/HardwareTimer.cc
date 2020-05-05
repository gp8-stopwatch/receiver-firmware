/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "HardwareTimer.h"
// TODO delete
#include "Debug.h"
#include "ErrorHandler.h"
#include <cstring>

// TODO dome ifdefs
HardwareTimer *HardwareTimer::timer1 = nullptr;
HardwareTimer *HardwareTimer::timer2 = nullptr;
HardwareTimer *HardwareTimer::timer3 = nullptr;
HardwareTimer *HardwareTimer::timer15 = nullptr;

HardwareTimer::HardwareTimer (TIM_TypeDef *instance, uint32_t prescaler, uint32_t period)
{
        channel[0] = nullptr;
        channel[1] = nullptr;
        channel[2] = nullptr;
        channel[3] = nullptr;

        // TODO ifdefs
        if (instance == TIM1) {
                timer1 = this;
        }
        else if (instance == TIM2) {
                timer2 = this;
        }
        else if (instance == TIM3) {
                timer3 = this;
        }
        else if (instance == TIM15) {
                timer15 = this;
        }

        htim.Instance = instance;

        /*
         * Divides internal clock (CK_INT). This internal clock for some timers is APB1 clock
         * (for example 42MHz on STM32f407) and for some others is APB2 (84 MHz respectively).
         *
         * WARNING 0! But remember, that if APBx prescaler (APB1 or APB2 in STM32F407) is something
         * other than 1, then CK_INT is multiplied by 2, so ith becomes 84 and 168MHz respectively!
         *
         * WARNING 1! Prescaler == 0 divides CK_INT by 1, Prescaler == 1 divides by 2 and so on.
         *
         * WARNING 2! Even though htim.Init.Prescaler is uint32_t, most of the timers are 16 bit!
         * Remember to not exceed 65535!
         */
        htim.Init.Prescaler = prescaler;

        /*
         * This is the value we are counting to (incluting this value, so set 999 you want 1000
         * cycles). The register is called ARR.
         *
         * WARNING! Even though htim.Init.Period is uint32_t, most of the timers are 16 bit!
         * Remember to not exceed 65535!
         */
        htim.Init.Period = period;
        htim.Init.CounterMode = TIM_COUNTERMODE_UP;

        // Only 3 values are possible here : divide by 1, 2 or 4.
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim.Init.RepetitionCounter = 0;
        htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        clkEnable ();

        /*
         * HAL_TIM_Base_Init, HAL_TIM_OC_Init and HAL_TIM_IC_Init are almost the same, the only
         * difference being the MSP routines which I don't use anyway.
         */
        if (HAL_TIM_Base_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Start_IT (&htim) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

void HardwareTimer::clkEnable (TIM_HandleTypeDef *timer)
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
        else if (timer->Instance == TIM15) {
                __HAL_RCC_TIM15_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void HardwareTimer::clkDisable (TIM_HandleTypeDef *timer)
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
        else if (timer->Instance == TIM15) {
                __HAL_RCC_TIM15_CLK_DISABLE ();
        }
}

/*****************************************************************************/

void HardwareTimer::enableChannels (uint8_t channels)
{
        TIM_OC_InitTypeDef sConfigOC;
        sConfigOC.OCMode = TIM_OCMODE_TIMING;
        sConfigOC.Pulse = 0;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
        sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        if (channels & CHANNEL1) {
                if (HAL_TIM_OC_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_OC_Start_IT (&htim, TIM_CHANNEL_1);
        }

        if (channels & CHANNEL2) {
                if (HAL_TIM_OC_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_OC_Start_IT (&htim, TIM_CHANNEL_2);
        }

        if (channels & CHANNEL3) {
                if (HAL_TIM_OC_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_OC_Start_IT (&htim, TIM_CHANNEL_3);
        }

        if (channels & CHANNEL4) {
                if (HAL_TIM_OC_ConfigChannel (&htim, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
                        Error_Handler ();
                }

                HAL_TIM_OC_Start_IT (&htim, TIM_CHANNEL_4);
        }
}

/*****************************************************************************/

void HardwareTimer::setDuty (uint8_t channels, uint32_t duty)
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

/*****************************************************************************/

void HardwareTimer::serviceIrq (HardwareTimer *that)
{
        if (!that) {
                return;
        }

        TIM_HandleTypeDef *htim = &that->htim;
        TIM_TypeDef *instance = htim->Instance;

        /*+-------------------------------------------------------------------------+*/
        /*| Channel 1                                                               |*/
        /*+-------------------------------------------------------------------------+*/

        // This reacts both to OutputCompare and InputCapture events depending which mode is currently in operation
        // Checks if a) particular interupt is enabled b) if the interrupt is pending
        if (instance->DIER & TIM_IT_CC1 && instance->SR & TIM_FLAG_CC1) {
                // Clears the interrupt
                instance->SR = ~TIM_FLAG_CC1;

                if (!that->channel[0] || !that->channel[0]->onIrq) {
                        return;
                }

                // Input capture event
                if (htim->Instance->CCMR1 & TIM_CCMR1_CC1S) {
                        // HAL_TIM_IC_CaptureCallback (htim);
                        that->channel[0]->onIrq ();
                }
                // Output compare event
                else {
                        // HAL_TIM_OC_DelayElapsedCallback (htim);
                        // HAL_TIM_PWM_PulseFinishedCallback(htim);
                        that->channel[0]->onIrq ();
                }
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Channel 2                                                               |*/
        /*+-------------------------------------------------------------------------+*/

        if (instance->DIER & TIM_IT_CC2 && instance->SR & TIM_FLAG_CC2) {
                instance->SR = ~TIM_FLAG_CC2;

                if (!that->channel[1] || !that->channel[1]->onIrq) {
                        return;
                }

                if (htim->Instance->CCMR1 & TIM_CCMR1_CC2S) {
                        that->channel[1]->onIrq ();
                }
                else {
                        that->channel[1]->onIrq ();
                }
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Channel 3                                                               |*/
        /*+-------------------------------------------------------------------------+*/

        if (instance->DIER & TIM_IT_CC3 && instance->SR & TIM_FLAG_CC3) {
                instance->SR = ~TIM_FLAG_CC3;

                if (!that->channel[2] || !that->channel[2]->onIrq) {
                        return;
                }

                if (htim->Instance->CCMR2 & TIM_CCMR2_CC3S) {
                        that->channel[2]->onIrq ();
                }
                else {
                        that->channel[2]->onIrq ();
                }
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Channel 4                                                               |*/
        /*+-------------------------------------------------------------------------+*/

        if (instance->DIER & TIM_IT_CC4 && instance->SR & TIM_FLAG_CC4) {
                instance->SR = ~TIM_FLAG_CC4;

                if (!that->channel[3] || !that->channel[3]->onIrq) {
                        return;
                }

                if (htim->Instance->CCMR2 & TIM_CCMR2_CC4S) {
                        that->channel[3]->onIrq ();
                }
                else {
                        that->channel[3]->onIrq ();
                }
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Update event (UEV) from timer base.                                     |*/
        /*+-------------------------------------------------------------------------+*/

        // Timer update event (UEV)
        if (instance->DIER & TIM_IT_UPDATE && instance->SR & TIM_FLAG_UPDATE) {
                // Clears the interrupt
                instance->SR = ~TIM_IT_UPDATE;

                if (!that->onUpdate) {
                        return;
                }

                that->onUpdate ();
        }
}

/*****************************************************************************/

// TODO przenieść do plików HAL, zaimplementować dla F0 i F4, zaimplementować inne zegary niż TIM3
extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler () { HardwareTimer::serviceIrq (HardwareTimer::timer1); }
extern "C" void TIM2_IRQHandler () { HardwareTimer::serviceIrq (HardwareTimer::timer2); }
extern "C" void TIM3_IRQHandler () { HardwareTimer::serviceIrq (HardwareTimer::timer3); }
extern "C" void TIM15_IRQHandler () { HardwareTimer::serviceIrq (HardwareTimer::timer15); }

/*****************************************************************************/

AbstractTimerChannel::AbstractTimerChannel (HardwareTimer *timer, int channelNumber) { timer->setChannel (channelNumber, this); }

int AbstractTimerChannel::channelNumberToHal (int number) { return number * 4; }

/*****************************************************************************/

InputCaptureChannel::InputCaptureChannel (HardwareTimer *timer, int channelNumber, bool withIrq) : AbstractTimerChannel (timer, channelNumber)
{
        // Dotycząca timera
        TIM_IC_InitTypeDef sICConfig;
        sICConfig.ICPolarity = TIM_ICPOLARITY_RISING;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICFilter = 0;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;

        if (HAL_TIM_IC_ConfigChannel (&timer->htim, &sICConfig, channelNumberToHal (channelNumber)) != HAL_OK) {
                Error_Handler ();
        }

        if (withIrq) {
                if (HAL_TIM_IC_Start_IT (&timer->htim, channelNumberToHal (channelNumber)) != HAL_OK) {
                        Error_Handler ();
                }
        }
        else {
                if (HAL_TIM_IC_Start (&timer->htim, channelNumberToHal (channelNumber)) != HAL_OK) {
                        Error_Handler ();
                }
        }
}
