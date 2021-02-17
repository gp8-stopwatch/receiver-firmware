/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Config.h"

namespace cfg {

bool &changed ()
{
        static bool h = false; /// Says if the system should pay attention for settings that has been changed.
        return h;
}

/****************************************************************************/

template <typename T> T def (T t, T defaultValue)
{
        constexpr int mxVal = std::numeric_limits<T>::max ();

        if (t == mxVal) {
                return defaultValue;
        }

        return t;
}

/****************************************************************************/

void Config::restoreDefaults ()
{
        blindTime = def (blindTime, DEFAULT_BLIND_TIME_MS);
        dutyTresholdPercent = def (dutyTresholdPercent, DEFAULT_DUTY_TRESHOLD_PERCENT);
        minTreggerEventMs = def (minTreggerEventMs, DEFAULT_MIN_TRIGGER_EVENT_MS);
        noiseEventsPerTimeUnitHigh = def (noiseEventsPerTimeUnitHigh, DEFAULT_NOISE_EVENTS_PER_TIME_UNIT_HIGH);
        noiseEventsPerTimeUnitLow = def (noiseEventsPerTimeUnitLow, DEFAULT_NOISE_EVENTS_PER_TIME_UNIT_LOW);
}

/****************************************************************************/

template <typename T> T trim (T t)
{
        constexpr int mxVal = std::numeric_limits<T>::max ();

        if (t == mxVal) {
                return mxVal - 1;
        }

        return t;
}

/****************************************************************************/

void Config::setBlindTime (uint16_t b) { blindTime = trim (b); }
void Config::setDutyTresholdPercent (uint8_t i) { dutyTresholdPercent = trim (i); }
void Config::setMinTreggerEventMs (uint16_t i) { minTreggerEventMs = trim (i); }
void Config::setNoiseEventsPerTimeUnitHigh (uint16_t i) { noiseEventsPerTimeUnitHigh = trim (i); }
void Config::setNoiseEventsPerTimeUnitLow (uint16_t i) { noiseEventsPerTimeUnitLow = trim (i); }

} // namespace cfg