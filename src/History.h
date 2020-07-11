/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Types.h"
#include <cstdint>
#include <etl/queue.h>
#include <limits>
#include <storage/ICircullarQueueStorage.h>
#include <storage/IRandomAccessStorage.h>

class History {
public:
        static constexpr size_t MAX_RESULTS_NUM = 64;

        void store (Result t);
        void run ();

        Result getHiScore () const { return hiScore; }
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
        etl::queue<Result, FLASH_QUEUE_SIZE> flashQueue;
};
