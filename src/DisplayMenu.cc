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
        option = Option ((int (option) + 1) % int (Option::LAST_OPTION));
        prepareMenuForOption (option);
}

/*****************************************************************************/

void DisplayMenu::onLongPress ()
{
        switch (option) {
        case Option::STOP_WATCH:
                break;

        case Option::SCREEN_ORIENTATON:
                config.orientationFlip = !config.orientationFlip;
                break;

        case Option::IR_ON:
                config.irSensorOn = !config.irSensorOn;
                break;

        case Option::BUZZER_ON:
                config.buzzerOn = !config.buzzerOn;
                break;

        default:
                break;
        }

        prepareMenuForOption (option);
        cfg::changed () = true;
}

/*****************************************************************************/

void DisplayMenu::prepareMenuForOption (Option o)
{
        display.clear ();

        switch (o) {
        case Option::STOP_WATCH:
                machine.resume ();
                break;

        case Option::SCREEN_ORIENTATON:
                machine.pause ();
                display.setText ("1.FLIP");
                break;

        case Option::IR_ON:
                if (config.irSensorOn) {
                        display.setText ("2.I.r.on");
                }
                else {
                        display.setText ("2.I.r.off");
                }

                break;

        case Option::BUZZER_ON:
                if (config.buzzerOn) {
                        display.setText ("3.Sn.on");
                }
                else {
                        display.setText ("3.Sn.off");
                }

                break;

        default:
                break;
        }
}
