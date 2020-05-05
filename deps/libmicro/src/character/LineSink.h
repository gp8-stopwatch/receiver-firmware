/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LINESINK_H
#define LINESINK_H

#include "BinaryEvent.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "character/ICharacterSink.h"
#include <cctype>
#include <cstdint>

/**
 * Observes an character source like Usart and groups characters received into
 * lines. Every line is then placed on a Queue.
 */
template <typename QueueT, typename EventT> class LineSink : public ICharacterSink {
public:
        using EventType = EventT;

        LineSink (QueueT &g) : gsmQueue (g) {}
        virtual ~LineSink () = default;

        virtual void onData (uint8_t c) override;
        virtual void onError (uint32_t) { Error_Handler (); }

        /**
         * If set, sink will not split nest n lines.
         */
        void receiveLines (size_t n);

//#define ALL_DATA_DEBUG
#ifdef ALL_DATA_DEBUG
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

protected:
        uint16_t rxBufferGsmPos = 0;
        uint8_t rxBufferGsm[128]; /// Bufor wejściowy na odpowiedzi z modemu. Mamy własny, gdyż kolejka może się w między czasie wyczyścić.
        QueueT &gsmQueue;
};

/*****************************************************************************/

template <typename QueueT, typename EventT> void LineSink<QueueT, EventT>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG
        addAllData (c);
#endif

        if (!std::isprint (c) && c != '\r' && c != '\n') {
                return;
        }

        if (rxBufferGsmPos > 0 && (c == '\r' || c == '\n')) {

                rxBufferGsm[rxBufferGsmPos] = '\0';

                // Nie wrzucamy na kolejkę odpowiedzi, które zawierają tylko \r\n
                if (rxBufferGsmPos > 1) {
#if 0
                        debug->print ("rx : ");
                        debug->println (rxBufferGsm, rxBufferGsmPos);
#endif

                        if (!gsmQueue.push_back ()) {
                                Error_Handler ();
                        }

                        auto &queueBuffer = gsmQueue.back ();
                        // TODO Optimize - for example etl::queue has an emplace method.
                        // TODO 64 hardcoded.
                        // TODO kolejka powinna przyjmować stringi o zmiennej długości.
                        if (rxBufferGsmPos > BINARY_EVENT_MAX_SIZE /*EventType::capacity ()*/) {
                                Error_Handler ();
                        }

                        queueBuffer = EventType (rxBufferGsm, rxBufferGsm + rxBufferGsmPos);
                }

                rxBufferGsmPos = 0;
        }
        else if (std::isprint (c)) {
                rxBufferGsm[rxBufferGsmPos++] = c;
        }
}

#endif // LINESINK_H
