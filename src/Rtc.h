/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include <cstdint>

/**
 * @brief The Rtc class
 */
class Rtc {
public:
        void init ();
        void deinit ();
        void activateWakeup (uint32_t timeUnits);
        void deactivateWakeup ();
        // void setDate (MicroDate d);
        void getDate () const;
        void backupRegisterWrite (uint32_t backupRegister, uint32_t data);
        uint32_t backupRegisterRead (uint32_t backupRegister) const;
};
