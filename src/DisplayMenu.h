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
enum class Event { shortPress, longPress, timePassed, refreshMenu };
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

        DisplayMenu (cfg::Config &c, IDisplay &d, FastStateMachine &m) : config{c}, display{d}, machine{m} {}

        void onEvent (menu::Event p);
        void setHistory (History *h) { this->history = h; }
        void setRtc (Rtc *r) { this->rtc = r; }

private:
        void prepareMenuForOption (Option o);

        cfg::Config &config;
        IDisplay &display;
        FastStateMachine &machine;
        Rtc *rtc{};
        History *history{};
};
