/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "catch.hpp"
#include "character/ICharacterSink.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

class MySink : public ICharacterSink {
public:
        //        MySink (StringQueue &g) : gsmQueue (g) {}
        virtual ~MySink () {}
        virtual void onData (char c);
        virtual void onError (uint32_t flags){};

private:
        uint16_t rxBufferGsmPos = 0;
        char rxBufferGsm[128]; /// Bufor wejściowy na odpowiedzi z modemu. Mamy własny, gdyż kolejka może się w między czasie wyczyścić.
                               //        StringQueue &gsmQueue;

        char allData[256];
        uint8_t allDataCnt = 0;
};

/*****************************************************************************/

void MySink::onData (char c)
{
        if (!std::isprint (c) && c != '\r' && c != '\n') {
                return;
        }

        if (rxBufferGsmPos > 0 && (c == '\r' || c == '\n')) {

                rxBufferGsm[rxBufferGsmPos] = '\0';

                // Nie wrzucamy na kolejkę odpowiedzi, które zawierają tylko \r\n
                if (rxBufferGsmPos > 1) {
#if 1
                        std::cerr << "rx : " << rxBufferGsm << std::endl;
#endif

                        //  __disable_irq ();

                        //                        if (!gsmQueue.push_back ()) {
                        //                                Error_Handler ();
                        //                        }

                        // char *queueBuffer = &gsmQueue.back ();
                        // strcpy (queueBuffer, rxBufferGsm);
                        std::cerr << "strcpy (queueBuffer, rxBufferGsm)" << std::endl;

                        // __enable_irq ();
                }

                rxBufferGsmPos = 0;
        }
        else if (std::isprint (c)) {
                rxBufferGsm[rxBufferGsmPos++] = c;
                allData[allDataCnt++] = c;
        }
}

TEST_CASE ("MySink", "[sourceSink]")
{
        // REQUIRE (avg.getResult () == Approx (0.0f));
        MySink sink;
        sink.onData ('A');
        sink.onData ('T');
        sink.onData ('+');
        sink.onData ('I');
        sink.onData ('P');
        sink.onData ('R');
        sink.onData ('=');
        sink.onData ('1');
        sink.onData ('1');
        sink.onData ('5');
        sink.onData ('2');
        sink.onData ('0');
        sink.onData ('0');
        sink.onData ('\r');
        sink.onData ('\r');
        sink.onData ('\n');
        sink.onData ('O');
        sink.onData ('K');
        sink.onData ('\r');
        sink.onData ('\n');
}
