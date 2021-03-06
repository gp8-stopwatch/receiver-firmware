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
#include <etl/string.h>

#if defined(PLATFORM_MICRO)
#include "platform/Micro.h"
#elif defined(PLATFORM_REGULAR)
#include "platform/Regular.h"
#elif defined(PLATFORM_HUGE)
#include "platform/Huge.h"
#endif

using String = etl::string<16>;
// using Result1us = uint64_t;   /// 1µs units. Maybe someday I'll use std::chrono
// using Result1usLS = uint32_t; /// 1µs units. Least signifficant part of Result1us
using Result10us = uint32_t; /// 10µs units.

/**
 * std::chrono would be much better, but the binary size and code speed
 * would have to be tested.
 */
template <typename T> class ResultT {
public:
        constexpr ResultT (T i = 0) : t{i} {}
        constexpr ResultT operator- (ResultT const &b) const { return ResultT{t - b.t}; }
        constexpr ResultT operator+ (ResultT const &b) const { return ResultT{t + b.t}; }
        constexpr ResultT operator/ (ResultT const &b) const { return ResultT{t / b.t}; }
        constexpr ResultT operator* (ResultT const &b) const { return ResultT{t * b.t}; }

        constexpr bool operator== (ResultT const &b) const { return t == b.t; }
        constexpr bool operator< (ResultT const &b) const { return t < b.t; }
        constexpr bool operator<= (ResultT const &b) const { return t <= b.t; }
        constexpr bool operator> (ResultT const &b) const { return t > b.t; }
        constexpr bool operator>= (ResultT const &b) const { return t >= b.t; }

        constexpr ResultT &operator<<= (int i)
        {
                t <<= i;
                return *this;
        }

        constexpr ResultT &operator|= (ResultT const &b)
        {
                t |= b.t;
                return *this;
        }

        constexpr explicit operator T () const { return t; }

private:
        T t;
};

using Result1us = ResultT<uint64_t>;
using Result1usLS = ResultT<uint32_t>;

constexpr Result10us result1To10 (Result1us r) { return uint64_t (r) / 10 + ((uint64_t (r) % 10 < 5) ? (0) : (1)); }
constexpr Result1us msToResult1us (uint32_t r) { return {r * 1000}; }
constexpr Result1usLS resultLS (Result1us r) { return {static_cast<uint32_t> (static_cast<uint64_t> (r) & 0x0000'0000'ffff'ffffULL)}; }
constexpr Result1us lsResult (Result1usLS r) { return Result1us{uint32_t (r)}; }

/**
 * How to display a result.
 */
enum ResultDisplayStyle { secondFraction = 0b00, milisecondOnly = 0b01, none = 0b11 };

constexpr size_t CAN_BUS_PRIORITY = 3; // CAN bus is no longer used for time synchronization, so low priority
constexpr size_t BUTTON_EXTI_PRIORITY = 1;
constexpr size_t IR_EXTI_PRIORITY = 1;
constexpr size_t EXT_TRIGGER_INPUT_EXTI_PRIORITY = 1;
constexpr size_t SYS_TICK_PRIORITY = 4;

inline const char *VERSION = "1.1.0";

constexpr int LOW_VOLTAGE_MV = 3000;
constexpr int LOW_VOLTAGE_CRITICAL_MV = 2900;
constexpr int RESPONSE_WAIT_TIME_MS = 1000;
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
enum Brightness { level0 = 0, level1 = 1, level2 = 2, level3 = 3, level4 = 4, levelAuto = 0b111 };
enum ParticipantsNumber { one = 1, two = 0 };
enum StopMode { stop = 1, restart = 0 }; // Loop

enum class NoiseState { noNoise, noise };
enum class BeamState { present, absent };

/*****************************************************************************/

struct InfoRespData {
        uint32_t uid{};
        DeviceType deviceType{};
        bool active{};
        BeamState beamState{};
        NoiseState noiseState{};
        uint8_t noiseLevel{};
};

/*****************************************************************************/

static constexpr uint16_t DEFAULT_BLIND_TIME_MS = 5000;
static constexpr uint16_t DEFAULT_MIN_TRIGGER_EVENT_MS = 10;
static constexpr int NO_IR_DETECTED_MS = 5000;
static constexpr int LOOP_DISPLAY_TIMEOUT = DEFAULT_BLIND_TIME_MS - 1000;
static constexpr uint8_t DEFAULT_NOISE_LEVEL_HIGH = 3;
static constexpr uint8_t DEFAULT_NOISE_LEVEL_LOW = 1;
static constexpr uint8_t MAX_NOISE_LEVEL = 15;
static constexpr uint16_t DEFAULT_FPS = 30;
static constexpr uint16_t MIN_FPS = 20;
static constexpr uint16_t MAX_FPS = 1000; // TODO this had to be lowered fropm 10000 down to 1000 to avoid TIM1 / TIM15 synchronization problem
static constexpr int DUTY_CYCLE_LOW_DIV = 8;  // Transition to PWM low state when duty cycle lower than 1/DUTY_CYCLE_LOW_DIV (12.5%)
static constexpr int DUTY_CYCLE_HIGH_DIV = 4; // Transition to PWM high state when duty cycle greater than 1/DUTY_CYCLE_HIGH_DIV (25%)
static constexpr Result1usLS EXT_TRIGGER_DURATION_US = 10000;
static constexpr Result1usLS EXT_TRIGGER_DURATION_RX_US = EXT_TRIGGER_DURATION_US / 2;
/// How often to calculate if noise state has changed.
static constexpr uint32_t NOISE_CALCULATION_PERIOD_MS = 1000;
static constexpr uint16_t NO_BEAM_CALCULATION_PERIOD_MS = 1000;

#ifndef UNIT_TEST
const etl::function_fv<__disable_irq> lock{};
const etl::function_fv<__enable_irq> unlock{};
#endif

#endif // __cplusplus
