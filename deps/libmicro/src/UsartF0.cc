/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "HalStm32F0.h"
#include "Usart.h"

/*****************************************************************************/

extern "C" void USART1_IRQHandler () { Usart::fireOnData (Usart::usart1); }

/*****************************************************************************/

extern "C" void USART2_IRQHandler () { Usart::fireOnData (Usart::usart2); }

/*****************************************************************************/

// TODO it shouldn't be like that
#if !defined (STM32F042x6)
extern "C" void USART3_4_IRQHandler ()
{
        Usart::fireOnData (Usart::usart3);
        Usart::fireOnData (Usart::usart4);
}
#endif
