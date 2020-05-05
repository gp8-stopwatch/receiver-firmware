/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Hal.h"
#include "Usart.h"

/*****************************************************************************/

extern "C" void USART1_IRQHandler () { Usart::fireOnData (Usart::usart1); }
extern "C" void USART2_IRQHandler () { Usart::fireOnData (Usart::usart2); }
extern "C" void USART3_IRQHandler () { Usart::fireOnData (Usart::usart3); }
extern "C" void UART4_IRQHandler () { Usart::fireOnData (Usart::usart4); }
extern "C" void UART5_IRQHandler () { Usart::fireOnData (Usart::usart5); }
extern "C" void USART6_IRQHandler () { Usart::fireOnData (Usart::usart6); }
#ifdef LIB_MICRO_STM32H
extern "C" void UART7_IRQHandler () { Usart::fireOnData (Usart::usart7); }
#endif
