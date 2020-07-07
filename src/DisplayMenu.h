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
                stop_watch, // Normal stopwatch operation i.e. time is displayed. The main screen.
                flip,       // Whether display is flipped horizontally or not.
                ir_on,      // Whether IR sensor i active or not.
                buzzer_on,
                resolution,
                stopMode,
                last_option
        };

        DisplayMenu (cfg::Config &c, IDisplay &d, FastStateMachine &m) : config (c), display (d), machine (m) {}

        void onShortPress ();
        void onLongPress ();

private:
        void prepareMenuForOption (Option o);

private:
        cfg::Config &config;
        IDisplay &display;
        FastStateMachine &machine;
        Option option = Option::stop_watch;
};
