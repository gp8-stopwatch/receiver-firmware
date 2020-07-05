/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Rtc.h"
#include <cstdint>
#include <limits>
#include <storage/ICircullarQueueStorage.h>
#include <storage/IRandomAccessStorage.h>

class History {
public:
        static constexpr size_t MAX_RESULTS_NUM = 64;

        struct Entry {
                RTC_DateTypeDef date;
                Time time;
                uint32_t result;
        };

        explicit History (Rtc &r) : rtc{r} {}

        void store (uint32_t t);

        uint32_t getHiScore () const { return hiScore; }
        void printHistory ();
        void printLast ();

        void setHistoryStorage (ICircullarQueueStorage *value) { historyStorage = value; }
        void setHiScoreStorage (IRandomAccessStorage *value)
        {
                hiScoreStorage = value;
                hiScore = *reinterpret_cast<uint32_t const *> (hiScoreStorage->read (nullptr, sizeof (uint32_t), 0));
        }

        void clearHiScore ();
        void clearResults ();

private:
        void storeHiScoreIf (uint32_t t);

private:
        uint32_t hiScore = std::numeric_limits<uint32_t>::max ();
        IRandomAccessStorage *hiScoreStorage = nullptr;
        ICircullarQueueStorage *historyStorage = nullptr;
        Rtc &rtc;
};
