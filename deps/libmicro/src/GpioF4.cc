/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Hal.h"
#include "Gpio.h"

extern "C" void EXTI0_IRQHandler ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_0);
        Gpio::connectedExtis[0]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI1_IRQHandler ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_1);
        Gpio::connectedExtis[1]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI2_IRQHandler ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_2);
        Gpio::connectedExtis[2]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI3_IRQHandler ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_3);
        Gpio::connectedExtis[3]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI4_IRQHandler ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_4);
        Gpio::connectedExtis[4]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI9_5_IRQHandler ()
{
        uint8_t pinNo;

        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_5) != RESET) {
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

        Gpio::connectedExtis[pinNo]->onToggle ();
}

/*****************************************************************************/

extern "C" void EXTI15_10_IRQHandler ()
{
        uint8_t pinNo;

        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_10) != RESET) {
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

        Gpio::connectedExtis[pinNo]->onToggle ();
}

