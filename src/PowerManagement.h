/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Adc.h"
#include "AdcChannel.h"
#include "Gpio.h"
#include "Hal.h"

class PowerManagement {
public:
        PowerManagement ();

        void run ();

        unsigned int getBatteryVoltage () const { return lastBatteryVoltage; }
        unsigned int getAmbientLight () const { return lastAmbientLight; }

private:
        Adc adc{2};
        AdcChannel ambientLightVoltMeter{GPIOA, GPIO_PIN_4, ADC_CHANNEL_4};
        AdcChannel batteryVoltMeter{GPIOB, GPIO_PIN_0, ADC_CHANNEL_8};
        Gpio chargeInProgress{GPIOB, GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLUP};
        Gpio chargeComplete{GPIOB, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLUP};
        Gpio senseOn{GPIOB, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
        unsigned int lastBatteryVoltage{};
        unsigned int lastAmbientLight{};
};