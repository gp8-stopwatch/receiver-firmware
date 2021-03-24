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

#ifdef WITH_HISTORY

class History {
public:
        static constexpr size_t MAX_RESULTS_NUM = 64;
        explicit History (Rtc &r) : rtc{r} {}

        void store (Result10us t);
        void run ();

        struct Entry {
                RTC_DateTypeDef date;
                Time time;
                Result10us result;
        };

        Result10us getHiScore () const { return hiScore; }
        History::Entry getEntry (size_t index) const;

        void printHistory (ResultDisplayStyle ra = ResultDisplayStyle::secondFraction) const;
        void printLast (ResultDisplayStyle ra = ResultDisplayStyle::secondFraction) const;

        void setHistoryStorage (ICircullarQueueStorage *value) { historyStorage = value; }
        void setHiScoreStorage (IRandomAccessStorage *value)
        {
                hiScoreStorage = value;
                hiScore = *reinterpret_cast<Result10us const *> (hiScoreStorage->read (nullptr, sizeof (Result10us), 0));
        }

        void clearHiScore ();
        void clearResults ();

private:
        void storeHiScoreIf (Result10us t);

        Result10us hiScore = std::numeric_limits<Result10us>::max ();
        IRandomAccessStorage *hiScoreStorage = nullptr;
        ICircullarQueueStorage *historyStorage = nullptr;
        etl::queue_spsc_locked<Result10us, FLASH_QUEUE_SIZE, etl::memory_model::MEMORY_MODEL_SMALL> flashQueue{lock, unlock};
        Rtc &rtc;
};

#else

class History {
public:
        static constexpr size_t MAX_RESULTS_NUM = 64;
        explicit History (Rtc & /* r */) {}

        void store (Result10us t) {}
        void run () {}

        struct Entry {
                RTC_DateTypeDef date;
                Time time;
                Result10us result;
        };

        Result10us getHiScore () const { return 0; }
        History::Entry getEntry (size_t index) const { return {}; }

        void printHistory (ResultDisplayStyle ra = ResultDisplayStyle::secondFraction) const {}
        void printLast (ResultDisplayStyle ra = ResultDisplayStyle::secondFraction) const {}

        void setHistoryStorage (ICircullarQueueStorage * /* value */) {}
        void setHiScoreStorage (IRandomAccessStorage * /* value */) {}

        void clearHiScore () {}
        void clearResults () {}
};

#endif
