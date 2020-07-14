/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Rtc.h"
#include "Types.h"
#include <cstdint>
#include <etl/function.h>
#include <etl/queue_spsc_locked.h>
#include <limits>
#include <storage/ICircullarQueueStorage.h>
#include <storage/IRandomAccessStorage.h>

class History {
public:
        static constexpr size_t MAX_RESULTS_NUM = 64;
        explicit History (Rtc &r) : rtc{r} {}

        void store (Result t);
        void run ();

        struct Entry {
                RTC_DateTypeDef date;
                Time time;
                Result result;
        };

        Result getHiScore () const { return hiScore; }
        History::Entry getEntry (size_t index) const;

        void printHistory ();
        void printLast ();

        void setHistoryStorage (ICircullarQueueStorage *value) { historyStorage = value; }
        void setHiScoreStorage (IRandomAccessStorage *value)
        {
                hiScoreStorage = value;
                hiScore = *reinterpret_cast<Result const *> (hiScoreStorage->read (nullptr, sizeof (Result), 0));
        }

        void clearHiScore ();
        void clearResults ();

private:
        void storeHiScoreIf (Result t);

        Result hiScore = std::numeric_limits<Result>::max ();
        IRandomAccessStorage *hiScoreStorage = nullptr;
        ICircullarQueueStorage *historyStorage = nullptr;
        etl::queue_spsc_locked<Result, FLASH_QUEUE_SIZE, etl::memory_model::MEMORY_MODEL_SMALL> flashQueue{lock, unlock};
        Rtc &rtc;
};