/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_CAN_MAINFILE_H
#define LIB_MICRO_CAN_MAINFILE_H

#include "Hal.h"

#if defined (LIB_MICRO_STM32F)
#include "CanF.h"
#elif defined (LIB_MICRO_NRG)
#include "CanNRG.h"
#elif defined (UNIT_TEST)
#include "CanUnit.h"
#endif

#endif // SPI_H
