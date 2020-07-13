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
#include <tuple>

/*****************************************************************************/

void print (int i)
{
        char buf[11];
        itoa (i, buf);
        usbWrite (buf);
}

/****************************************************************************/

void printResult (Result time)
{
        char buf[11];
        uint32_t sec100 = time % 100000;

        time /= 100000;
        uint32_t sec = time % 60;

        time /= 60;
        uint32_t min = time % 60;

        itoa ((unsigned int)(min), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(sec), buf, 2);
        usbWrite (buf);
        usbWrite (".");

        itoa ((unsigned int)(sec100), buf, 5);
        usbWrite (buf);
}

/*****************************************************************************/

void printDate (RTC_DateTypeDef const &date, Time const &time)
{
        char buf[11];
        itoa ((unsigned int)(date.Year + 2000), buf, 4);
        usbWrite (buf);
        usbWrite ("-");

        itoa ((unsigned int)(date.Month), buf, 2);
        usbWrite (buf);
        usbWrite ("-");

        itoa ((unsigned int)(date.Date), buf, 2);
        usbWrite (buf);
        usbWrite (" ");

        itoa ((unsigned int)(time.Hours), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(time.Minutes), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(time.Seconds), buf, 2);
        usbWrite (buf);
}

/****************************************************************************/

void printEntry (History::Entry const &en)
{
        printDate (en.date, en.time);
        usbWrite (" ");
        printResult (en.result);
}

/**
 * Useful in case when in flash wasnt cleared properly.
 */
History::Entry &fixEntry (History::Entry &e)
{
        e.date.Date = std::min<uint8_t> (e.date.Date, 31);
        e.date.Month = std::min<uint8_t> (e.date.Month, 12);
        e.time.Hours = std::min<uint8_t> (e.time.Hours, 24);
        e.time.Minutes = std::min<uint8_t> (e.time.Minutes, 60);
        e.time.Seconds = std::min<uint8_t> (e.time.Seconds, 24);
        return e;
}

/*****************************************************************************/

void History::store (uint32_t t) { flashQueue.push (t); }

/*****************************************************************************/

void History::storeHiScoreIf (uint32_t t)
{
        if (t < hiScore) {
                hiScore = t;
                hiScoreStorage->store (reinterpret_cast<uint8_t *> (&t), sizeof (t), 0);
        }
}

/*****************************************************************************/

void History::run ()
{
        // TODO use queue_spc_locked
        while (!flashQueue.empty ()) {
                Result t = flashQueue.front ();
                flashQueue.pop ();
                // historyStorage->store (reinterpret_cast<uint8_t *> (&t), sizeof (t), 0);

                History::Entry entry{};
                std::tie (entry.date, entry.time) = rtc.getDate ();
                entry.result = t;
                historyStorage->store (reinterpret_cast<uint8_t *> (&entry), sizeof (entry), 0);

                storeHiScoreIf (t);
        }
}

/*****************************************************************************/

void History::printHistory ()
{
        usbWrite ("Best ");

        if (hiScore != std::numeric_limits<uint32_t>::max ()) {
                printResult (hiScore);
        }
        else {
                printResult (0);
        }

        usbWrite ("\r\n");
        usbWrite ("\r\n");

        bool newLine{};
        int cnt = 0;
        for (int i = MAX_RESULTS_NUM - 1; i >= 0; --i) {
                Entry en = *reinterpret_cast<Entry const *> (historyStorage->read (nullptr, sizeof (Entry), 0, i));
                fixEntry (en);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        ++cnt;
                        print (cnt);
                        usbWrite (" ");
                        printEntry (en);
                        usbWrite ("\r\n");
                        newLine = true;
                }
        }

        if (newLine) {
                usbWrite ("\r\n"); // Only if results were there
        }
}

/*****************************************************************************/

void History::printLast ()
{
        std::optional<Entry> last{};

        for (int i = MAX_RESULTS_NUM - 1; i >= 0; --i) {
                Entry en = *reinterpret_cast<Entry const *> (historyStorage->read (nullptr, sizeof (Entry), 0, i));
                fixEntry (en);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        last = en;
                }
        }

        if (last) {
                printEntry (*last);
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
