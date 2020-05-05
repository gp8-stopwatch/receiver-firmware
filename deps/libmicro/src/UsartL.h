/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_UART_L_H
#define LIB_MICRO_UART_L_H

#include "Hal.h"
#include "character/ICharacterInput.h"
#include "character/ICharacterOutput.h"
#include <functional>

/**
 * @brief The Uart class
 */
class Usart : public ICharacterInput, public ICharacterOutput {
public:
        Usart (USART_TypeDef *instance, uint32_t baudRate);
        virtual ~Usart () override;

        static void clkEnable (USART_TypeDef *instance);
        static void clkDisable (USART_TypeDef *instance);
        void clkEnable () { clkEnable (huart.Instance); }
        void clkDisable () { clkDisable (huart.Instance); }

        void transmit (const uint8_t *str, size_t len);
        void transmit (const char *str, size_t len) override;
        void transmit (const char *str) override;

        /// Turns on the data ready IRQ and sets the callback.
        void startReceive ();

        void setSink (ICharacterSink *s) override { sink = s; }
        void setOnData (std::function<void(uint8_t)> const &t) { onData = t; }

        /// Turns off all IRQs of this usart
        void stopReceive ();

        /// Turns off data ready IRQs of this usart
        void pause () override;

        /// Turns on the data ready IRQ.
        void resume () override;

private:
        FRIEND_ALL_USART_IRQS
        UART_HandleTypeDef huart;

#if defined(USE_USART1) || defined(USE_UART1)
        static Usart *usart1;
#endif
#if defined(USE_USART2) || defined(USE_UART2)
        static Usart *usart2;
#endif
#if defined(USE_USART3) || defined(USE_UART3)
        static Usart *usart3;
#endif
#if defined(USE_USART4) || defined(USE_UART4)
        static Usart *usart4;
#endif
#if defined(USE_USART5) || defined(USE_UART5)
        static Usart *usart5;
#endif
#if defined(USE_USART6) || defined(USE_UART6)
        static Usart *usart6;
#endif

        static void fireOnData (Usart *u);
        std::function<void(uint8_t)> onData;
        ICharacterSink *sink;
};

#endif // UART_H
