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
enum OperationMode {
        LOOP = 0,  // First interruption of IR starts the timer, and every consecutive one resets it, but does not stop it.
        NORMAL = 1 // First break in IR barrier turns the stopwatch ON, second OFF and so on
};

enum ContestantsNumber { TWO = 0, ONE = 1 };
enum Resolution { us_10 = 0, us_100 = 1, ms_1 = 2, ms_10 = 3 };
enum Brightness { level1 = 0, level2 = 1, level3 = 2, level4 = 3, level5 = 4, levelAuto = 0b111 };
enum ParticipantsNumber { participants1 = 1, participants2 = 0 };
enum StopMode { stop = 1, restart = 0 }; // Loop

/**
 * System wide configuration. Available to the user and stored in the flash.
 */
struct Config {

        Config ()
        {
                mode = OperationMode::NORMAL;
                contestantsNum = ContestantsNumber::ONE;
                resolution = Resolution::ms_10;
                brightness = Brightness::levelAuto;
                participantsNumber = ParticipantsNumber::participants1;
                stopMode = StopMode::stop;
                // size_t beamInterruptionEventMs{5000}; // Blind period after IR trigger

                orientationFlip = true;
                irSensorOn = true;
                buzzerOn = true;
        }

        OperationMode mode : 1;
        ContestantsNumber contestantsNum : 1;
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
