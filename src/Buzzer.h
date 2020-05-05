/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BUZZER_H
#define BUZZER_H

#include "Gpio.h"
#include "Timer.h"
#include <cstdint>
#include <stm32f0xx_hal.h>

class Buzzer {
public:
        Buzzer (Gpio &pin) : pin (pin) { pin = false; }

        void beep (uint16_t periodOn, uint16_t periodOff, uint8_t times);
        void run ();
        bool isActive () const { return active; }
        void setActive (bool b) { active = b; }

private:
        void on () { pin = true; }
        void off () { pin = false; }

private:
        Gpio &pin;
        Timer timer;
        uint16_t periodOn = 0;
        uint16_t periodOff = 0;
        uint8_t setTimes = 0;
        uint8_t elapsedTimes = 0;
        bool soundState = true;
        bool active = true;
};

#endif // BUZZER_H
