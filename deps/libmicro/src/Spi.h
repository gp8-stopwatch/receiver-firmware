/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_SPI_H
#define LIB_MICRO_SPI_H

#include "Hal.h"

#if defined(LIB_MICRO_STM32F) | defined(LIB_MICRO_STM32L)
#include "SpiF.h"
#elif defined(LIB_MICRO_NRG)
#include "SpiNRG.h"
#endif

#endif // SPI_H
