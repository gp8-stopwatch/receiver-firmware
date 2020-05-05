/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Gpio.h"
#include "Hal.h"
#include "Timer.h"
#include <cstdint>

class Button {
public:
        explicit Button (Gpio &pin);

        bool getPressClear () const;
        bool getLongPressClear () const;
        void run ();

private:
        void onToggle ();

private:
        Gpio &pin;

        mutable bool pressedEvent = false;
        mutable bool longPressedEvent = false;
        mutable bool pressed = false;

        Timer longPressTimer;
        Timer debounceTimer;
};
