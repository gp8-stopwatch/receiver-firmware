/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_COMMON_MICRO_UART_H
#define LIB_COMMON_MICRO_UART_H

#include <Hal.h>
#include <functional>

#if defined (LIB_MICRO_STM32F)
#include "UsartF.h"

#elif defined (LIB_MICRO_STM32L)
#include "UsartL.h"

#elif defined (LIB_MICRO_NRG)
#include "UsartNRG.h"

#elif defined (X86_TEST)
#include "UsartX86T.h"
#endif

#endif // UART_H
