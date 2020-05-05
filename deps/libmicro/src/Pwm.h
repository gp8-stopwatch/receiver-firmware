/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_PWM_H
#define LIB_MICRO_PWM_H

#include "Hal.h"

class Pwm {
public:
        Pwm (TIM_TypeDef *instance, uint32_t prescaler, uint32_t period);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkEnable (TIM_HandleTypeDef *timer);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkDisable (TIM_HandleTypeDef *timer);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (&htim); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (&htim); }

        enum Channel { CHANNEL1 = 0x01, CHANNEL2 = 0x02, CHANNEL3 = 0x04, CHANNEL4 = 0x08 };

        /**
         * Use bitmask Channel enum values to enable channels.
         * @param channels
         */
        void enableChannels (uint8_t channels);

        void setDuty (uint8_t channel, uint32_t duty);

private:
        TIM_HandleTypeDef htim;
};

#endif // PWM_H
