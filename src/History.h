/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef GP8_HISTORY_H
#define GP8_HISTORY_H

#include <cstdint>
#include <limits>
#include <storage/ICircullarQueueStorage.h>
#include <storage/IRandomAccessStorage.h>

class History {
public:
        void store (uint32_t t);

        uint32_t getHiScore () const { return hiScore; }
        void printHistory ();

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
};

#endif // HISTORY_H
