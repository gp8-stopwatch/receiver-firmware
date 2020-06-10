/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "PowerManagement.h"

/****************************************************************************/

PowerManagement::PowerManagement ()
{
        adc.addChannel (&ambientLightVoltMeter);
        adc.addChannel (&batteryVoltMeter);
        senseOn = false;
}

void PowerManagement::run ()
{
        senseOn = true;
        HAL_Delay (1);
        adc.run ();
        lastBatteryVoltage = batteryVoltMeter.getValue ();
        lastAmbientLight = ambientLightVoltMeter.getValue ();
        senseOn = false;

        //                        if (batteryVoltage <= 125) {
        //                                screen->setBatteryLevel (1);
        //                        }
        //                        else if (batteryVoltage <= 130) {
        //                                screen->setBatteryLevel (2);
        //                        }
        //                        else if (batteryVoltage <= 140) {
        //                                screen->setBatteryLevel (3);
        //                        }
        //                        else if (batteryVoltage <= 148) {
        //                                screen->setBatteryLevel (4);
        //                        }
        //                        else {
        //                                screen->setBatteryLevel (5);
        //                        }
}