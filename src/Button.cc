/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Button.h"

Button::Button (Gpio &pin) : pin (pin)
{
        // pin.setOnToggle ([this] { onToggle (); });
}

/*****************************************************************************/

void Button::onToggle ()
{
        if (!debounceTimer.isExpired ()) {
                return;
        }
        debounceTimer.start (50);

        // Button pressed (button gpio is normally pulled up)
        if (pin) {
                longPressTimer.start (1000);
                pressed = true;
        }
        else {
                if (!pressed) {
                        return;
                }

                pressed = false;
                pressedEvent = true;
        }
}

/*****************************************************************************/

bool Button::getPressClear () const
{
        if (pressedEvent) {
                pressedEvent = false;
                return true;
        }

        return false;
}

/*****************************************************************************/

bool Button::getLongPressClear () const
{
        if (longPressedEvent) {
                longPressedEvent = false;
                return true;
        }

        return false;
}

/*****************************************************************************/

void Button::run ()
{
        if (pressed && longPressTimer.isExpired ()) {
                longPressedEvent = true;
                pressed = false;
        }
}
