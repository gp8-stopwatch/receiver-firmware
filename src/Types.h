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
#include "Hal.h"
#include <cstdint>
#include <etl/cstring.h>

#if defined(PLATFORM_MICRO)
#include "platform/Micro.h"
#elif defined(PLATFORM_REGULAR)
#include "platform/Regular.h"
#elif defined(PLATFORM_HUGE)
#include "platform/Huge.h"
#endif

using String = etl::string<16>;
using Result = uint32_t; /// Results are in 10µs units. Maybe someday I'll use std::chrono

constexpr size_t DISPLAY_TIMER_PRIORITY = 2;
constexpr size_t CAN_BUS_PRIORITY = 1; // CAN bus is no longer used for time synchronization, so low priority
constexpr size_t BUTTON_EXTI_PRIORITY = 1;
constexpr size_t IR_EXTI_PRIORITY = 1;
constexpr size_t EXT_TRIGGER_INPUT_EXTI_PRIORITY = 1;

inline const char *VERSION = "1.0.4";

constexpr int LOW_VOLTAGE_MV = 3000;
constexpr int LOW_VOLTAGE_CRITICAL_MV = 2900;
constexpr int RESPONSE_WAIT_TIME_MS = 100;
constexpr int CAN_SEND_TIMEOUT = 50;

/**
 * Values are chosen so after loading from empty flash (0xff everywhere), everyting would
 * have some meaningfull value.
 */
enum OperationMode {
        LOOP = 0,  // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
        NORMAL = 1 // First break in IR barrier turns the stopwatch ON, second OFF and so on
};

enum class DeviceType : uint8_t {
        receiver,     // Normal receiver with display
        ir_sensor,    // Micro reciver wthout the display, battery nor any buttons.
        huge_display, // 5 inch 7 segment display
};

#ifdef PLATFORM_REGULAR
const DeviceType myDeviceType = DeviceType::receiver;
#endif

#ifdef PLATFORM_MICRO
const DeviceType myDeviceType = DeviceType::ir_sensor;
#endif

#ifdef PLATFORM_HUGE
const DeviceType myDeviceType = DeviceType::huge_display;
#endif

static constexpr int RESOLUTION_NUMBER_OF_OPTIONS = 4;
enum Resolution { us_10 = 0, us_100 = 1, ms_1 = 2, ms_10 = 3 };
enum Brightness { level1 = 0, level2 = 1, level3 = 2, level4 = 3, level5 = 4, levelAuto = 0b111 };
enum ParticipantsNumber { one = 1, two = 0 };
enum StopMode { stop = 1, restart = 0 }; // Loop

static constexpr int DEFAULT_BLIND_TIME_MS = 5000;
static constexpr int NO_IR_DETECTED_MS = 5000;
static constexpr int LOOP_DISPLAY_TIMEOUT = DEFAULT_BLIND_TIME_MS - 1000;

const etl::function_fv<__disable_irq> lock{};
const etl::function_fv<__enable_irq> unlock{};

#endif // __cplusplus
