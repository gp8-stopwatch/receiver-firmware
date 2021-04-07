/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Micro.h"
#include "Container.h"

/****************************************************************************/

extern "C" void EXTI4_15_IRQHandler ()
{
        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_5) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_5);
                getIrDetector ().onEdge ({getStopWatch ().getTimeFromIsr ()}, EdgePolarity (getIrTriggerInput ().get ()));
        }
        // else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_15) != RESET) {
        //         __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_15);
        //         getButton ().onToggle ();
        // }
}

/****************************************************************************/

extern "C" void EXTI0_1_IRQHandler ()
{
        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_0) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_0);
                getExtDetector ().onEdge ({getStopWatch ().getTime ()}, EdgePolarity (getExtTriggerInput ().get ()));
        }
}
