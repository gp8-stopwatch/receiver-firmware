/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#ifdef __cplusplus
#include <etl/cstring.h>
#include <exception>
#endif

// This gets included in a C file, so ifdefs.
#define USB_PRIORITY 3

#ifdef __cplusplus
constexpr size_t DISPLAY_TIMER_PRIORITY = 2;
constexpr size_t CAN_BUS_PRIORITY = 1;
constexpr size_t BUTTON_AND_IR_EXTI_PRIORITY = 1;
constexpr size_t TEST_TRIGGER_EXTI_PRIORITY = 1;
constexpr size_t STOPWATCH_PRIORITY = 0;

static const char *VERSION = "1.0";

using String = etl::string<16>;

enum class OperationMode {
        NORMAL, // First break in IR barrier turns the stopwatch ON, second OFF and so on
        LOOP    // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
};

enum class ContestantsNumber { ONE = 1, TWO };
enum class Resolution { ms_10, ms_1, us_100, us_10 };
enum class Brightness { levelAuto, level1, level2, level3, level4, level5 };
enum class ParticipantsNumber { participants1, participants2 };
enum class StopMode { stop, restart }; // Loop

/**
 * System wide configuration. Available to the user and stored in the flash.
 */
struct Config {

        OperationMode mode = OperationMode::NORMAL;
        ContestantsNumber contestantsNum = ContestantsNumber::ONE;
        Resolution resolution{Resolution::us_10};
        Brightness brightness{Brightness::levelAuto};
        ParticipantsNumber participantsNumber;
        StopMode stopMode;

        bool orientationFlip = false;
        bool irSensorOn = true;
        bool buzzerOn = true;
        bool hasChanged = false; /// Says if the system should pay attention for settings that has been changed. Not user-settable.
};

#endif
