/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once

#include "FastStateMachine.h"
#include "Gpio.h"
#include "Hal.h"
#include "IDisplay.h"
#include "Timer.h"
#include <cstdint>

class PowerManagement {
public:
        enum class Channels { ambient, battery, /* temperature, */ vref };
        static constexpr uint32_t RANGE_12BITS = 4095;

        PowerManagement (IDisplay &d, FastStateMachine &m);

        void run ();
        void sleep ();

        unsigned int getBatteryVoltage () const { return lastBatteryVoltage; }
        unsigned int getBatteryPercent () const { return std::min<unsigned int> ((lastBatteryVoltage - 2900) / 12, 100); }
        unsigned int getAmbientLight () const { return lastAmbientLight; }

private:
        ADC_HandleTypeDef hadc{};
        Gpio chargeInProgress{GPIOB, GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_NOPULL};
        Gpio chargeComplete{GPIOB, GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_NOPULL};
        Gpio senseOn{GPIOB, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
        uint32_t lastBatteryVoltage{};
        uint32_t lastAmbientLight{};
        uint32_t lastTemperature{};
        uint32_t VREFINT_DATA{};
        IDisplay &display;
        FastStateMachine &machine;
        Timer batteryTimer;
};