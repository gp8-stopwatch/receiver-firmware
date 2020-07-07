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
