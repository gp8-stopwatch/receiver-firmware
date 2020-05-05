/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/
#pragma once

struct Config {
        enum class OperationMode {
                NORMAL, // First break in IR barrier turns the stopwatch ON, second OFF and so on
                LOOP    // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
        };

        OperationMode mode = OperationMode::NORMAL;

        enum class ContestantsNumber { ONE = 1, TWO };
        ContestantsNumber contestantsNum = ContestantsNumber::ONE;

        bool orientationFlip = false;
        bool irSensorOn = true;
        bool buzzerOn = true;

        /// Says if the system should pay attention for settings that has been changed.
        bool hasChanged = false;
};
