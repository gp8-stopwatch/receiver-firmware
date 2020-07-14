/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Container.h"
#include "FastStateMachine.h"
#include "History.h"
#include "IDisplay.h"
#include "Machine.h"
#include "Rtc.h"

namespace menu {
enum class Event { shortPress, longPress, timePassed };
}

class DisplayMenu {
public:
        enum class Option {
                stop_watch, // Normal stopwatch operation i.e. time is displayed. The main screen.
                result,     // Cycle through all the 64 results
                flip,       // Whether display is flipped horizontally or not.
                ir_on,      // Whether IR sensor i active or not.
                buzzer_on,
                resolution,
                stopMode,
                time,
                date,
                last_option // Guard.
        };

        DisplayMenu (cfg::Config &c, IDisplay &d, FastStateMachine &m, Rtc &rtc, History &h)
            : config{c}, display{d}, machine{m}, rtc{rtc}, history (h)
        {
        }

        void onEvent (menu::Event p);

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
        History &history;
        Timer timeDisplay;
        int settingState{}; // Some menu options require turning stopwatch into a state. Like when setting the time.
};
