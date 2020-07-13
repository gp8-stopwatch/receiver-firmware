/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Hal.h"
#include "Rtc.h"
#include "Types.h"
#include "usbd_cdc.h"

void print (int i);
void printResult (Result time);
void printDate (RTC_DateTypeDef const &date, Time const &time);
