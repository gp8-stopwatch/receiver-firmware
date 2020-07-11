/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "DisplayMenu.h"

/*****************************************************************************/

void DisplayMenu::onShortPress ()
{
        // Cycle the menu.
        option = Option ((int (option) + 1) % int (Option::last_option));
        prepareMenuForOption (option);
}

/*****************************************************************************/

void DisplayMenu::onLongPress ()
{
        switch (option) {
        case Option::stop_watch:
                break;

        case Option::flip:
                config.orientationFlip = !config.orientationFlip;
                break;

        case Option::ir_on:
                config.irSensorOn = !config.irSensorOn;
                break;

        case Option::buzzer_on:
                config.buzzerOn = !config.buzzerOn;
                break;

        case Option::resolution:
                config.resolution = Resolution ((int (config.resolution) - 1) % RESOLUTION_NUMBER_OF_OPTIONS);
                break;

        case Option::stopMode:
                config.stopMode = StopMode (!bool (config.stopMode));
                break;

        default:
                break;
        }

        prepareMenuForOption (option);

        if (option != Option::stop_watch) {
                cfg::changed () = true;
        }
}

/*****************************************************************************/

void DisplayMenu::prepareMenuForOption (Option o)
{
        display.clear ();

        if (o == Option::stop_watch) {
                machine.run (Event::reset);
        }
        else {
                machine.run (Event::pause);
        }

        switch (o) {
        case Option::stop_watch:
                break;

        case Option::flip:
                display.setText ("1.FLIP");
                break;

        case Option::ir_on:
                if (config.irSensorOn) {
                        display.setText ("2.I.r.on");
                }
                else {
                        display.setText ("2.I.r.off");
                }

                break;

        case Option::buzzer_on:
                if (config.buzzerOn) {
                        display.setText ("3.Sn.on");
                }
                else {
                        display.setText ("3.Sn.off");
                }
                break;

        case Option::resolution:
                switch (config.resolution) {
                case Resolution::ms_10:
                        display.setText ("4.ms.10");
                        break;

                case Resolution::ms_1:
                        display.setText ("4.ms.1");
                        break;

                case Resolution::us_100:
                        display.setText ("4.us.100");
                        break;

                case Resolution::us_10:
                        display.setText ("4.us.10");
                        break;

                default:
                        break;
                }

                break;

        case Option::stopMode:
                if (config.stopMode == StopMode::stop) {
                        display.setText ("5.Stop");
                }
                else {
                        display.setText ("5.loop");
                }
                break;

        default:
                break;
        }
}
