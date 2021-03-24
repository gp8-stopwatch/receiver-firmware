/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Hal.h"
#include <cstdint>
#include <utility>

/// ST Cube time structure is too big.
struct Time {

        uint8_t Hours; /*!< Specifies the RTC Time Hour.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected  */

        uint8_t Minutes; /*!< Specifies the RTC Time Minutes.
                              This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

        uint8_t Seconds; /*!< Specifies the RTC Time Seconds.
                              This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

        uint8_t TimeFormat; /*!< Specifies the RTC AM/PM Time.
                                 This parameter can be a value of @ref RTC_AM_PM_Definitions */
};

#ifdef WITH_RTC

/**
 * @brief The Rtc class
 */
class Rtc {
public:
        Rtc ();
        void activateWakeup (uint32_t timeUnits);
        void deactivateWakeup ();

        // void setDate (MicroDate d);
        std::pair<RTC_DateTypeDef, Time> getDate () const;

        void backupRegisterWrite (uint32_t backupRegister, uint32_t data);
        uint32_t backupRegisterRead (uint32_t backupRegister) const;

        enum class Set { none, hour, minute, second, year, month, day };

        void timeAdd (Set set);
        void dateAdd (Set set);

private:
        void RTC_CalendarConfig ();

        mutable RTC_HandleTypeDef rtcHandle;
};

#else

class Rtc {
public:
        Rtc () = default;
        void activateWakeup (uint32_t timeUnits) {}
        void deactivateWakeup () {}

        // void setDate (MicroDate d);
        std::pair<RTC_DateTypeDef, Time> getDate () const { return {}; }

        void backupRegisterWrite (uint32_t backupRegister, uint32_t data) {}
        uint32_t backupRegisterRead (uint32_t /* backupRegister */) const { return 0; }

        enum class Set { none, hour, minute, second, year, month, day };

        void timeAdd (Set set) {}
        void dateAdd (Set set) {}
};

#endif