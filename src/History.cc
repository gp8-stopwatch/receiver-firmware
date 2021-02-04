/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "History.h"
#include "Debug.h"
#include "usb/UsbHelpers.h"
#include "usbd_cdc.h"
#include <algorithm>
#include <stm32f0xx_hal.h>
#include <tuple>

/****************************************************************************/

void printEntry (History::Entry const &en, ResultDisplayStyle ra, bool datePrint = true)
{
        if (datePrint) {
                printDate (en.date, en.time);
                usbWrite (" ");
        }
        printResult (en.result, ra);
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

void History::store (uint32_t t) { flashQueue.push_from_unlocked (t); }

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
        Result t{};
        while (flashQueue.pop (t)) {
                History::Entry entry{};
                std::tie (entry.date, entry.time) = rtc.getDate ();
                entry.result = t;
                historyStorage->store (reinterpret_cast<uint8_t *> (&entry), sizeof (entry), 0);
                storeHiScoreIf (t);
        }
}

/*****************************************************************************/

void History::printHistory (ResultDisplayStyle ra) const
{
        usbWrite ("Best ");

        if (hiScore != std::numeric_limits<uint32_t>::max ()) {
                printResult (hiScore, ra);
        }
        else {
                printResult (0, ra);
        }

        usbWrite ("\r\n");
        usbWrite ("\r\n");

        bool newLine{};
        int cnt = 0;
        for (int i = MAX_RESULTS_NUM - 1; i >= 0; --i) {
                Entry en = getEntry (i);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        ++cnt;
                        print (cnt);
                        usbWrite (" ");
                        printEntry (en, ra);
                        usbWrite ("\r\n");
                        newLine = true;
                }
        }

        if (newLine) {
                usbWrite ("\r\n"); // Only if results were there
        }
}

/*****************************************************************************/

History::Entry History::getEntry (size_t index) const
{
        index %= MAX_RESULTS_NUM;
        Entry en = *reinterpret_cast<Entry const *> (historyStorage->read (nullptr, sizeof (Entry), 0, index));
        fixEntry (en);
        return en;
}

/*****************************************************************************/

void History::printLast (ResultDisplayStyle ra) const
{
        std::optional<Entry> last{};

        for (int i = MAX_RESULTS_NUM - 1; i >= 0; --i) {
                Entry en = getEntry (i);

                if (en.result != std::numeric_limits<uint32_t>::max ()) {
                        last = en;
                }
        }

        if (last) {
                printEntry (*last, ra, false);
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
