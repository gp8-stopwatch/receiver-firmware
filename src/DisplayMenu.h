/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "FastStateMachine.h"
#include "IDisplay.h"

class DisplayMenu {
public:
        enum class Option {
                STOP_WATCH,        // Normal stopwatch operation i.e. time is displayed. The main screen.
                SCREEN_ORIENTATON, // Whether display is flipped horizontally or not.
                IR_ON,             // Whether IR sensor i active or not.
                BUZZER_ON,
                LAST_OPTION
        };

        DisplayMenu (Config &c, IDisplay &d, FastStateMachine &m) : config (c), display (d), machine (m) {}

        void onShortPress ();
        void onLongPress ();

private:
        void prepareMenuForOption (Option o);

private:
        Config &config;
        IDisplay &display;
        FastStateMachine &machine;
        Option option = Option::STOP_WATCH;
};
