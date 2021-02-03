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
class Config {
public:
        Config ()
        {
                resolution = Resolution::ms_10;
                brightness = Brightness::levelAuto;
                participantsNumber = ParticipantsNumber::one;
                stopMode = StopMode::restart;

                displayRightSideUp = true;
                irSensorOn = true;
                buzzerOn = true;
                padding = 0;
                blindTime = DEFAULT_BLIND_TIME_MS;
        }

        Resolution getResolution () const { return resolution; }
        void setResolution (Resolution r) { resolution = r; }

        Brightness getBrightness () const { return brightness; }
        void setBrightness (Brightness b) { brightness = b; }

        ParticipantsNumber getParticipantsNumber () const { return participantsNumber; }
        void setParticipantsNumber (ParticipantsNumber p) { participantsNumber = p; }

        StopMode getStopMode () const { return stopMode; }
        void setStopMode (StopMode s) { stopMode = s; }

        bool isFlip () const { return displayRightSideUp; }
        void setFlip (bool d) { displayRightSideUp = d; }

        bool isIrSensorOn () const { return irSensorOn; }
        void setIrSensorOn (bool i) { irSensorOn = i; }

        bool isBuzzerOn () const { return buzzerOn; }
        void setBuzzerOn (bool b) { buzzerOn = b; }

        uint16_t getBlindTime () const { return (blindTime == std::numeric_limits<uint16_t>::max ()) ? (DEFAULT_BLIND_TIME_MS) : (blindTime); }
        void setBlindTime (uint16_t b) { blindTime = b; }

private:
        Resolution resolution : 2;
        Brightness brightness : 3;
        ParticipantsNumber participantsNumber : 1;
        StopMode stopMode : 1;

        bool displayRightSideUp : 1;
        bool irSensorOn : 1;
        bool buzzerOn : 1;
        char padding : 4;
        uint16_t blindTime;
};

extern bool &changed ();

static_assert (sizeof (Config) == 4);

} // namespace cfg
