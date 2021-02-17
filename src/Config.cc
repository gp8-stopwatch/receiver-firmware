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
        noiseLevelHigh = def (noiseLevelHigh, DEFAULT_NOISE_LEVEL_HIGH);
        noiseLevelLow = def (noiseLevelLow, DEFAULT_NOISE_LEVEL_LOW);
}

/****************************************************************************/

// This is to avoid re-setting to default values in Config::restoreDefaults
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
void Config::setDutyTresholdPercent (uint8_t i) { dutyTresholdPercent = std::min<uint8_t> (i, 100); }
void Config::setMinTriggerEventMs (uint16_t i) { minTreggerEventMs = trim (i); }
void Config::setNoiseLevelHigh (uint8_t i) { noiseLevelHigh = std::min<uint8_t> (i, MAX_NOISE_LEVEL); }
void Config::setNoiseLevelLow (uint8_t i) { noiseLevelLow = std::min<uint8_t> (i, MAX_NOISE_LEVEL); }

} // namespace cfg