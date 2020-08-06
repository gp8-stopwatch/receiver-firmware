/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Types.h"
#include <exception>

namespace cfg {

/**
 * System wide configuration. Available to the user and stored in the flash.
 * All default settings have to have 0b1 binary value, because cleared flash
 * is all 0xff.
 */
struct Config {

        Config ()
        {
                resolution = Resolution::ms_10;
                brightness = Brightness::levelAuto;
                participantsNumber = ParticipantsNumber::one;
                stopMode = StopMode::restart;
                // size_t beamInterruptionEventMs{5000}; // Blind period after IR trigger

                displayRightSideUp = true;
                irSensorOn = true;
                buzzerOn = true;
        }

        Resolution resolution : 2;
        Brightness brightness : 3;
        ParticipantsNumber participantsNumber : 1;
        StopMode stopMode : 1;
        // size_t beamInterruptionEventMs{5000}; // Blind period after IR trigger

        bool displayRightSideUp : 1;
        bool irSensorOn : 1;
        bool buzzerOn : 1;

        char padding : 4;
};

extern bool &changed ();

static_assert (sizeof (Config) == 2);

} // namespace cfg
