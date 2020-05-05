/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "HalStm32L4.h"
#include "Usart.h"

/*****************************************************************************/

extern "C" void USART1_IRQHandler () { Usart::fireOnData (Usart::usart1); }
extern "C" void USART2_IRQHandler () { Usart::fireOnData (Usart::usart2); }
extern "C" void USART3_IRQHandler () { Usart::fireOnData (Usart::usart3); }
extern "C" void UART4_IRQHandler () { Usart::fireOnData (Usart::usart4); }
extern "C" void UART5_IRQHandler () { Usart::fireOnData (Usart::usart5); }
