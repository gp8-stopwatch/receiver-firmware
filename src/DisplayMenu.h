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
#include "Rtc.h"

class DisplayMenu {
public:
        enum class Option {
                stop_watch, // Normal stopwatch operation i.e. time is displayed. The main screen.
                flip,       // Whether display is flipped horizontally or not.
                ir_on,      // Whether IR sensor i active or not.
                buzzer_on,
                resolution,
                stopMode,
                time,
                date,
                last_option
        };

        DisplayMenu (cfg::Config &c, IDisplay &d, FastStateMachine &m, Rtc &rtc) : config{c}, display{d}, machine{m}, rtc{rtc} {}

        void onShortPress ();
        void onLongPress ();
        void run ();

private:
        void prepareMenuForOption (Option o);

        cfg::Config &config;
        IDisplay &display;
        FastStateMachine &machine;
        Option option = Option::stop_watch;
        Rtc &rtc;
        Timer timeDisplay;
        int settingState{}; // Some menu options require turning stopwatch into a state. Like when setting the time.
};
