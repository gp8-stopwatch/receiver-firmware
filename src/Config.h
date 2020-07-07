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

// TODO move somewhere?
using String = etl::string<16>;

// TODO move somewhere?
constexpr size_t DISPLAY_TIMER_PRIORITY = 2;
constexpr size_t CAN_BUS_PRIORITY = 1;
constexpr size_t BUTTON_AND_IR_EXTI_PRIORITY = 1;
constexpr size_t TEST_TRIGGER_EXTI_PRIORITY = 1;
constexpr size_t STOPWATCH_PRIORITY = 0;

// TODO move somewhere?
static const char *VERSION = "1.0.1";

namespace cfg {

/**
 * Values are chosen so after loading from empty flash (0xff everywhere), everyting would
 * have some meaningfull value.
 */
enum Resolution { us_10 = 0, us_100 = 1, ms_1 = 2, ms_10 = 3 };
enum Brightness { level1 = 0, level2 = 1, level3 = 2, level4 = 3, level5 = 4, levelAuto = 0b111 };

enum ParticipantsNumber {
        one = 1, // One contestant
        two = 0  // Two simultaneously
};

enum StopMode {
        stop = 1,   // (default) First break in IR barrier turns the stopwatch ON, second OFF and so on
        restart = 0 // (loop) First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
};

/**
 * System wide configuration. Available to the user and stored in the flash.
 */
struct Config {

        Config ()
        {
                resolution = Resolution::ms_10;
                brightness = Brightness::levelAuto;
                participantsNumber = ParticipantsNumber::one;
                stopMode = StopMode::stop;
                // size_t beamInterruptionEventMs{5000}; // Blind period after IR trigger

                orientationFlip = true;
                irSensorOn = true;
                buzzerOn = true;
        }

        Resolution resolution : 2;
        Brightness brightness : 3;
        ParticipantsNumber participantsNumber : 1;
        StopMode stopMode : 1;
        // size_t beamInterruptionEventMs{5000}; // Blind period after IR trigger

        bool orientationFlip : 1;
        bool irSensorOn : 1;
        bool buzzerOn : 1;

        char padding : 4;
};

extern bool &changed ();

static_assert (sizeof (Config) == 2);

} // namespace cfg
#endif
