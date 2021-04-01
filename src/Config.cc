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
        setBlindTime (def (blindTime, DEFAULT_BLIND_TIME_MS));
        setMinTriggerEventMs (def (minTriggerEventMs, DEFAULT_MIN_TRIGGER_EVENT_MS));
        setNoiseLevelHigh (def (noiseLevelHigh, DEFAULT_NOISE_LEVEL_HIGH));
        setNoiseLevelLow (def (noiseLevelLow, DEFAULT_NOISE_LEVEL_LOW));
        setFps (def (fps, DEFAULT_FPS));
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
// 0-65534 (0xffff-1)
void Config::setBlindTime (uint16_t b) { blindTime = trim (b); }

void Config::setMinTriggerEventMs (uint16_t i)
{
        minTriggerEventMs = trim (i);
        minTriggerEventMs = std::max<uint16_t> (i, 1);
}

void Config::setNoiseLevelHigh (uint8_t i) { noiseLevelHigh = std::min<uint8_t> (i, MAX_NOISE_LEVEL); }
void Config::setNoiseLevelLow (uint8_t i) { noiseLevelLow = std::min<uint8_t> (i, MAX_NOISE_LEVEL); }

/****************************************************************************/

void Config::setFps (uint16_t i)
{
        fps = trim (i);
        fps = std::max<uint16_t> (i, MIN_FPS);
        fps = std::min<uint16_t> (i, MAX_FPS);
}

} // namespace cfg