/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "History.h"
#include "Debug.h"
#include "usbd_cdc.h"
#include <algorithm>
#include <stm32f0xx_hal.h>

/*****************************************************************************/

void printTime (uint16_t time)
{
        char buf[6];
        uint16_t sec100 = time % 100;

        time /= 100;
        uint16_t sec = time % 60;

        time /= 60;
        uint16_t min = time % 60;

        itoa (min, buf);
        usbWrite (buf);
        usbWrite (":");

        itoa (sec, buf, 2);
        usbWrite (buf);
        usbWrite (",");

        itoa (sec100, buf, 2);
        usbWrite (buf);
}

/*****************************************************************************/

void History::store (uint16_t t)
{
        historyStorage->store (reinterpret_cast<uint8_t *> (&t), 2, 0);
        storeHiScoreIf (t);
}

/*****************************************************************************/

void History::storeHiScoreIf (uint16_t t)
{
        if (t < hiScore) {
                hiScore = t;
                hiScoreStorage->store (reinterpret_cast<uint8_t *> (&t), 2, 0);
        }
}

/*****************************************************************************/

void History::printHistory ()
{
        usbWrite ("Hi ");
        printTime (hiScore);
        usbWrite ("\r\n");
        usbWrite ("\r\n");

        for (int i = 63; i >= 0; --i) {
                uint16_t tim = *reinterpret_cast<uint16_t const *> (historyStorage->read (nullptr, sizeof (uint16_t), 0, i));
                printTime (tim);
                usbWrite ("\r\n");
        }

        usbWrite ("\r\n");
}

/*****************************************************************************/

void History::init () { hiScore = *reinterpret_cast<uint16_t const *> (hiScoreStorage->read (nullptr, sizeof (uint16_t), 0)); }

/*****************************************************************************/

void History::clearHiScore ()
{
        hiScoreStorage->clear ();
        hiScore = std::numeric_limits<uint16_t>::max ();
}

/*****************************************************************************/

void History::clearResults () { historyStorage->clear (); }
