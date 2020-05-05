/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_CANUNIT_H
#define LIB_MICRO_CANUNIT_H

#include "CanFrame.h"
#include "ICanCallback.h"
#include "Timer.h"
#include <thread>
#include <vector>

/**
 * CAN peripheral
 */
class Can {
public:
        /// Only for unit test purposes.
        struct Entry {
                Entry (uint32_t d, CanFrame const &cf) : delayMs (d), frame (cf) {}
                uint32_t delayMs;
                CanFrame frame;
        };

        Can (std::vector<Entry> const &entries, ICanCallback *c = nullptr);

        bool send (CanFrame const &buff);
        CanFrame read ();
        void setFilterAndMask (uint32_t filter, uint32_t mask, bool extended) {}
        void setCanCallback (ICanCallback *c);

private:
        void simulateTimeout () const;
        void startThread ();

private:
        std::vector<Entry> entries;
        Timer timer;
        int cnt;
        ICanCallback *callback;
        std::thread thread;
};

#endif // CANUNIT_H
