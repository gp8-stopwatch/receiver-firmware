/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
// This gets included in a C file, so ifdefs.
#define USB_PRIORITY 3

#ifdef __cplusplus
#include <cstdint>
#include <etl/cstring.h>

using String = etl::string<16>;
using Result = uint32_t; /// Results are in 10µs units. Maybe someday I'll use std::chrono

constexpr size_t DISPLAY_TIMER_PRIORITY = 2;
constexpr size_t CAN_BUS_PRIORITY = 1;
constexpr size_t BUTTON_AND_IR_EXTI_PRIORITY = 1;
constexpr size_t TEST_TRIGGER_EXTI_PRIORITY = 1;
constexpr size_t STOPWATCH_PRIORITY = 0;

static const char *VERSION = "1.0.3";

/**
 * Values are chosen so after loading from empty flash (0xff everywhere), everyting would
 * have some meaningfull value.
 */
enum OperationMode {
        LOOP = 0,  // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
        NORMAL = 1 // First break in IR barrier turns the stopwatch ON, second OFF and so on
};

static constexpr int RESOLUTION_NUMBER_OF_OPTIONS = 4;
enum Resolution { us_10 = 0, us_100 = 1, ms_1 = 2, ms_10 = 3 };
enum Brightness { level1 = 0, level2 = 1, level3 = 2, level4 = 3, level5 = 4, levelAuto = 0b111 };
enum ParticipantsNumber { one = 1, two = 0 };
enum StopMode { stop = 1, restart = 0 }; // Loop

static constexpr int BEAM_INTERRUPTION_EVENT = 5000;
static constexpr int NO_IR_DETECTED_MS = 5000;
static constexpr int LOOP_DISPLAY_TIMEOUT = BEAM_INTERRUPTION_EVENT - 1000;

#endif // __cplusplus
