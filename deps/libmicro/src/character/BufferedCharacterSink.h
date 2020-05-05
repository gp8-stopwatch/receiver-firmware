/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BUFFEREDCHARACTERSINK_H
#define BUFFEREDCHARACTERSINK_H

#include "ErrorHandler.h"
#include "Hal.h"
#include "ICharacterSink.h"
#include <cstdlib>
#include <etl/queue_spsc_isr.h>

template <size_t MAX_SIZE> class BufferedCharacterSink : public ICharacterSink {
public:
        using QueueType = etl::queue_spsc_isr<uint8_t, MAX_SIZE, CortexMInterruptControl>;

        BufferedCharacterSink (ICharacterSink &child) : childSink (child) {}
        BufferedCharacterSink (BufferedCharacterSink const &) = delete;
        BufferedCharacterSink &operator= (BufferedCharacterSink const &) = delete;
        BufferedCharacterSink (BufferedCharacterSink &&) = delete;
        BufferedCharacterSink &operator= (BufferedCharacterSink &&) = delete;
        virtual ~BufferedCharacterSink () = default;

        /// Called from ISR
        void onData (uint8_t c) override;

        /// Called from ISR
        void onError (uint32_t) override { Error_Handler (); }

        /// Call periodicaly from a main loop (not from ISR).
        void run ();

#define ALL_DATA_DEBUG_BUFFERED
#ifdef ALL_DATA_DEBUG_BUFFERED
        static constexpr size_t ALL_DATA_MAX_SIZE = 2048;
        uint8_t allData[ALL_DATA_MAX_SIZE];
        size_t allDataCnt = 0;

        void addAllData (uint8_t c)
        {
                if (allDataCnt < ALL_DATA_MAX_SIZE) {
                        allData[allDataCnt++] = c;
                }
        }
#endif

private:
        QueueType queue;
        ICharacterSink &childSink;
};

/*****************************************************************************/

template <size_t MAX_SIZE> inline void BufferedCharacterSink<MAX_SIZE>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG_BUFFERED
        addAllData (c);
#endif

        if (!queue.push_from_isr (c)) {
                // Too much data at once.
                Error_Handler ();
        }
}

/*****************************************************************************/

template <size_t MAX_SIZE> void BufferedCharacterSink<MAX_SIZE>::run ()
{
        //        if (queue.size () >= MAX_SIZE / 2) {
        //                while (!queue.empty ()) {
        //                        uint8_t c;
        //                        queue.pop (c);
        //                        childSink.onData (c);
        //                }
        //        }

        if (!queue.empty ()) {
                uint8_t c;
                queue.pop (c);
                childSink.onData (c);
        }
}

#endif // BUFFEREDCHARACTERSINK_H
