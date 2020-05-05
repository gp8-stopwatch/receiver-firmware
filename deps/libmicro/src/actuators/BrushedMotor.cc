/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "BrushedMotor.h"
#include "Gpio.h"
#include <algorithm>

void BrushedMotor::setSpeed (int32_t speed)
{
        int newDuty = speed * factor;
        newDuty = std::min<int> (std::abs (newDuty), fullScale);

        if (pwmInvert) {
                newDuty = fullScale - newDuty;
        }

        pwm->setDuty (channel, newDuty);
        direction->set (directionInvert ^ (speed > 0));
}
