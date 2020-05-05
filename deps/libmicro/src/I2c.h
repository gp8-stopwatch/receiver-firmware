/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_I2C_COMMON_FILE_H
#define LIB_MICRO_I2C_COMMON_FILE_H

#include "Hal.h"

#if defined(LIB_MICRO_STM32F0)
#include "I2cF0.h"
#elif defined(LIB_MICRO_STM32F4)
#include "I2cF4.h"
#elif defined(LIB_MICRO_NRG)
#include "I2cNRG.h"
#endif

#endif // I2C_H
