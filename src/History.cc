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

void printTime (uint32_t time)
{
        char buf[11];
        uint32_t sec100 = time % 100000;

        time /= 100000;
        uint32_t sec = time % 60;

        time /= 60;
        uint32_t min = time % 60;

        itoa ((unsigned int)(min), buf);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(sec), buf, 2);
        usbWrite (buf);
        usbWrite (",");

        itoa ((unsigned int)(sec100), buf, 2);
        usbWrite (buf);
}

/*****************************************************************************/

void History::store (uint32_t t)
{
        historyStorage->store (reinterpret_cast<uint8_t *> (&t), sizeof (t), 0);
        storeHiScoreIf (t);
}

/*****************************************************************************/

void History::storeHiScoreIf (uint32_t t)
{
        if (t < hiScore) {
                hiScore = t;
                hiScoreStorage->store (reinterpret_cast<uint8_t *> (&t), sizeof (t), 0);
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
                uint32_t tim = *reinterpret_cast<uint32_t const *> (historyStorage->read (nullptr, sizeof (uint32_t), 0, i));
                printTime (tim);
                usbWrite ("\r\n");
        }

        usbWrite ("\r\n");
}

/*****************************************************************************/

void History::clearHiScore ()
{
        hiScoreStorage->clear ();
        hiScore = std::numeric_limits<uint32_t>::max ();
}

/*****************************************************************************/

void History::clearResults () { historyStorage->clear (); }
