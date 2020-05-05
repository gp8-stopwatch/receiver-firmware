/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_UART_H
#define LIB_MICRO_UART_H

#include "Hal.h"
#include "character/ICharacterInput.h"
#include "character/ICharacterOutput.h"
#include <functional>

/**
 * @brief The Uart class
 */
class Usart : public ICharacterInput, public ICharacterOutput {
public:
        Usart (uint32_t baudRate);
        virtual ~Usart ();

        void transmit (const uint8_t *str, size_t len);
        void transmit (const char *str, size_t len);
        void transmit (const char *str);

        /// Turns on the data ready IRQ and sets the callback.
        //        void startReceive ();

        void setSink (ICharacterSink *s) { sink = s; }
        //        void setOnData (std::function<void(uint8_t)> const &t) { onData = t; }

        /// Turns off all IRQs of this usart
        //        void stopReceive ();

        /// Turns off data ready IRQs of this usart
        void pause () {}

        /// Turns on the data ready IRQ.
        void resume () {}

private:
        //        static void fireOnData (Usart *u);
        //        std::function<void(uint8_t)> onData;
        ICharacterSink *sink;
};

#endif // UART_H
