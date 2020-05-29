/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include <exception>

static const char *VERSION = "1.0";

enum class OperationMode {
        NORMAL, // First break in IR barrier turns the stopwatch ON, second OFF and so on
        LOOP    // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
};

enum class ContestantsNumber { ONE = 1, TWO };

enum class Resolution { ms_10, ms_1, ms_01 };

/**
 * System wide configuration. Available to the user and stored in the flash.
 */
struct Config {

        OperationMode mode = OperationMode::NORMAL;
        ContestantsNumber contestantsNum = ContestantsNumber::ONE;
        Resolution resolution{Resolution::ms_01};

        bool orientationFlip = false;
        bool irSensorOn = true;
        bool buzzerOn = true;
        bool hasChanged = false; /// Says if the system should pay attention for settings that has been changed. Not a setting.
};
