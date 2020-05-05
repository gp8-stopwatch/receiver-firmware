/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Buzzer.h"

/*****************************************************************************/

void Buzzer::beep (uint16_t periodOn, uint16_t periodOff, uint8_t times)
{
        if (!active) {
                return;
        }

        this->periodOn = periodOn;
        this->periodOff = periodOff;
        this->setTimes = times;
        this->elapsedTimes = 0;
        this->soundState = true;
        timer.start (periodOn);
        on ();
}

/*****************************************************************************/

void Buzzer::run ()
{
        if (elapsedTimes >= setTimes) {
                return;
        }

        if (!timer.isExpired ()) {
                return;
        }

        if (soundState) {
                off ();
                soundState = false;
                timer.start (periodOff);
                ++elapsedTimes;
        }
        else {
                on ();
                soundState = true;
                timer.start (periodOn);
        }
}
