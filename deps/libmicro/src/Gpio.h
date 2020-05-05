/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_GPIO_H
#define LIB_MICRO_GPIO_H

#include <Hal.h>
#include <functional>

#if defined (LIB_MICRO_STM32F)
#include "GpioF.h"

#elif defined (LIB_MICRO_STM32L)
#include "GpioL.h"

#elif defined (LIB_MICRO_NRG)
#include "GpioNRG.h"

#elif defined (X86_TEST)
#include "GpioX86T.h"
#endif

#endif // GPIO_H
