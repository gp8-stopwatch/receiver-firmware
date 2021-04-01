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
 * is all 0xff. Then restoreDefaults can be used to further properly reset
 * some values to their correcty defaults.
 */
class Config {
public:
#ifdef UNIT_TEST
        Config ()
        {
                resolution = Resolution::ms_10;
                brightness = Brightness::levelAuto;
                participantsNumber = ParticipantsNumber::one;
                stopMode = StopMode::restart;
                autoDisplayResult = ResultDisplayStyle::none;

                notFlipped = true;
                irSensorOn = true;
                buzzerOn = true;

                blindTime = DEFAULT_BLIND_TIME_MS;
                minTriggerEventMs = DEFAULT_MIN_TRIGGER_EVENT_MS;
                noiseLevelHigh = DEFAULT_NOISE_LEVEL_HIGH;
                noiseLevelLow = DEFAULT_NOISE_LEVEL_LOW;
                fps = DEFAULT_FPS;
        }
#endif

        /// Use after loading from flassh. If a value has 0b1111... value, then correct default value is set instead.
        void restoreDefaults ();

        Resolution getResolution () const { return resolution; }
        void setResolution (Resolution r) { resolution = r; }

        Brightness getBrightness () const { return brightness; }
        void setBrightness (Brightness b) { brightness = b; }

        ParticipantsNumber getParticipantsNumber () const { return participantsNumber; }
        void setParticipantsNumber (ParticipantsNumber p) { participantsNumber = p; }

        StopMode getStopMode () const { return stopMode; }
        void setStopMode (StopMode s) { stopMode = s; }

        ResultDisplayStyle getAutoDisplayResult () const { return autoDisplayResult; }
        void setAutoDisplayResult (ResultDisplayStyle rs) { autoDisplayResult = rs; }

        bool isFlip () const { return !notFlipped; }
        void setFlip (bool d) { notFlipped = !d; }

        bool isIrSensorOn () const { return irSensorOn; }
        void setIrSensorOn (bool i) { irSensorOn = i; }

        bool isBuzzerOn () const { return buzzerOn; }
        void setBuzzerOn (bool b) { buzzerOn = b; }

        uint16_t getBlindTime () const { return blindTime; }
        void setBlindTime (uint16_t b);

        uint16_t getMinTreggerEventMs () const { return minTriggerEventMs; }
        void setMinTriggerEventMs (uint16_t i);

        uint8_t getNoiseLevelHigh () const { return noiseLevelHigh; }
        void setNoiseLevelHigh (uint8_t i);

        uint8_t getNoiseLevelLow () const { return noiseLevelLow; }
        void setNoiseLevelLow (uint8_t i);

        uint16_t getFps () const { return fps; }
        void setFps (uint16_t i);

private:
        Resolution resolution : 2;
        Brightness brightness : 3;
        ParticipantsNumber participantsNumber : 1;
        StopMode stopMode : 1;
        ResultDisplayStyle autoDisplayResult : 2;

        bool notFlipped : 1;
        bool irSensorOn : 1;
        bool buzzerOn : 1;

        uint8_t reserved;

        uint16_t blindTime;
        uint16_t minTriggerEventMs;
        uint16_t fps;
        uint8_t noiseLevelHigh; // This number or more to report noise condition
        uint8_t noiseLevelLow;  // Less than this number to restore normal operation
};

extern bool &changed ();

// static_assert (sizeof (Config) == 4);
#ifndef UNIT_TEST
static_assert (sizeof (Config) == 12);
#endif

} // namespace cfg
