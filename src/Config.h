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

                dutyTresholdPercent = DEFAULT_DUTY_TRESHOLD_PERCENT;
                blindTime = DEFAULT_BLIND_TIME_MS;
                minTreggerEventMs = DEFAULT_MIN_TRIGGER_EVENT_MS;
                noiseEventsPerTimeUnitHigh = DEFAULT_NOISE_EVENTS_PER_TIME_UNIT_HIGH;
                noiseEventsPerTimeUnitLow = DEFAULT_NOISE_EVENTS_PER_TIME_UNIT_LOW;
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

        uint8_t getDutyTresholdPercent () const { return dutyTresholdPercent; }
        void setDutyTresholdPercent (uint8_t i);

        uint16_t getMinTreggerEventMs () const { return minTreggerEventMs; }
        void setMinTreggerEventMs (uint16_t i);

        uint16_t getNoiseEventsPerTimeUnitHigh () const { return noiseEventsPerTimeUnitHigh; }
        void setNoiseEventsPerTimeUnitHigh (uint16_t i);

        uint16_t getNoiseEventsPerTimeUnitLow () const { return noiseEventsPerTimeUnitLow; }
        void setNoiseEventsPerTimeUnitLow (uint16_t i);

private:
        Resolution resolution : 2;
        Brightness brightness : 3;
        ParticipantsNumber participantsNumber : 1;
        StopMode stopMode : 1;
        ResultDisplayStyle autoDisplayResult : 2;

        bool notFlipped : 1;
        bool irSensorOn : 1;
        bool buzzerOn : 1;

        uint8_t dutyTresholdPercent;

        uint16_t blindTime;
        uint16_t minTreggerEventMs;
        uint16_t noiseEventsPerTimeUnitHigh; // This number or more to report noise condition
        uint16_t noiseEventsPerTimeUnitLow;  // Less than this number to restore normal operation
};

extern bool &changed ();

// static_assert (sizeof (Config) == 4);
static_assert (sizeof (Config) == 12);

} // namespace cfg
