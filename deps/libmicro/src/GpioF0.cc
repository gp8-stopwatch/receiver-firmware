/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Gpio.h"
#include "HalStm32F0.h"

extern "C" void EXTI0_1_IRQHandler ()
{
        uint8_t pinNo = 0;

        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_0) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_0);
                pinNo = 0;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_1) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_1);
                pinNo = 1;
        }
        else {
                Error_Handler ();
        }

        Gpio::connectedExtis[pinNo]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI2_3_IRQHandler ()
{
        uint8_t pinNo = 0;

        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_2) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_2);
                pinNo = 2;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_3) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_3);
                pinNo = 3;
        }
        else {
                Error_Handler ();
        }

        Gpio::connectedExtis[pinNo]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI4_15_IRQHandler ()
{
        uint8_t pinNo = 0;

        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_4) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_4);
                pinNo = 4;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_5) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_5);
                pinNo = 5;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_6) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_6);
                pinNo = 6;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_7) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_7);
                pinNo = 7;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_8) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_8);
                pinNo = 8;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_9) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_9);
                pinNo = 9;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_10) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_10);
                pinNo = 10;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_11) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_11);
                pinNo = 11;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_12) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_12);
                pinNo = 12;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_13) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_13);
                pinNo = 13;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_14) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_14);
                pinNo = 14;
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_15) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_15);
                pinNo = 15;
        }
        else {
                Error_Handler ();
        }

        Gpio::connectedExtis[pinNo]->onToggle ();
}
