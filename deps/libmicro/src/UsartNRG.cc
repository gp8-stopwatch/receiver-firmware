/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Hal.h"
#include "Usart.h"
#include <cstring>

/*****************************************************************************/

Usart::Usart (uint32_t baudRate) : sink (nullptr)
{
        // - BaudRate = 115200 baud..
        // - Word Length = 8 Bits
        // - One Stop Bit
        // - No parity
        // - Hardware flow control disabled (RTS and CTS signals)
        // - Receive and transmit enabled
        UART_InitType uartInit;
        memset (&uartInit, 0, sizeof (uartInit));
        uartInit.UART_BaudRate = baudRate;
        uartInit.UART_WordLengthTransmit = UART_WordLength_8b;
        uartInit.UART_WordLengthReceive = UART_WordLength_8b;
        uartInit.UART_StopBits = UART_StopBits_1;
        uartInit.UART_Parity = UART_Parity_No;
        uartInit.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
        uartInit.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
        uartInit.UART_FifoEnable = ENABLE;
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_UART, ENABLE);
        UART_Init (&uartInit);

        /* Interrupt as soon as data is received. */
        UART_RxFifoIrqLevelConfig (FIFO_LEV_1_64);
        UART_Cmd (ENABLE);
}

/*****************************************************************************/

Usart::~Usart ()
{
        UART_Cmd (DISABLE);
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_UART, DISABLE);
}

/*****************************************************************************/

void Usart::transmit (const uint8_t *str, size_t len)
{
        for (size_t i = 0; i < len; ++i) {
                while (!UART_GetFlagStatus (UART_FLAG_TXFE)) {
                        ;
                }

                UART_SendData (*(str + i));
        }
}

/*****************************************************************************/

void Usart::transmit (const char *str, size_t len) { transmit (reinterpret_cast<uint8_t const *> (str), len); }

/*****************************************************************************/

void Usart::transmit (const char *str) { transmit (reinterpret_cast<uint8_t const *> (str), strlen (str)); }

        /*****************************************************************************/

        // void Usart::startReceive ()
        //{
        //        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        //        huart.Instance->CR3 |= USART_CR3_EIE;

        //        // Enable the UART Parity Error and Data Register not empty Interrupts
        //        huart.Instance->CR1 |= (USART_CR1_PEIE | USART_CR1_RXNEIE);
        //}

        /*****************************************************************************/

        // void Usart::stopReceive ()
        //{
        //        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        //        huart.Instance->CR3 &= ~USART_CR3_EIE;

        //        // Enable the UART Parity Error and Data Register not empty Interrupts
        //        huart.Instance->CR1 &= ~(USART_CR1_PEIE | USART_CR1_RXNEIE);
        //}

        /*****************************************************************************/

        // void Usart::pause () { huart.Instance->CR1 &= ~USART_CR1_RXNEIE; }

        /*****************************************************************************/

        // void Usart::resume () { huart.Instance->CR1 |= USART_CR1_RXNEIE; }

        /*****************************************************************************/

#if defined(LIB_MICRO_STM32F0)
void Usart::fireOnData (Usart *u)
{
        // If it were not initialized
        if (!u) {
                return;
        }

        UART_HandleTypeDef *huart = &u->huart;

        uint32_t isrflags = READ_REG (huart->Instance->ISR);
        uint32_t cr1its = READ_REG (huart->Instance->CR1);

        if (isrflags & uint32_t (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE)) {
                Error_Handler ();
        }

        // TODO ORE is silently discarded
        if (isrflags & uint32_t (USART_ISR_ORE)) {
                __HAL_USART_CLEAR_IT (huart, USART_CLEAR_OREF);
                return;
        }

        if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
                uint8_t c = (uint8_t) (huart->Instance->RDR & (uint8_t)0x00FF);

                if (u->onData) {
                        u->onData (c);
                }
                if (u->sink) {
                        u->sink->onData (char(c));
                }
        }
}
#elif defined(LIB_MICRO_STM32F4)
void Usart::fireOnData (Usart *u)
{
        // If it were not initialized
        if (!u) {
                return;
        }

        UART_HandleTypeDef *huart = &u->huart;

        uint32_t isrflags = READ_REG (huart->Instance->SR);
        uint32_t cr1its = READ_REG (huart->Instance->CR1);

        if (isrflags & uint32_t (USART_SR_PE | USART_SR_FE | USART_SR_NE)) {
                Error_Handler ();
        }

        // TODO ORE is silently discarded
        if (isrflags & uint32_t (USART_SR_ORE)) {
                //                __HAL_USART_CLEAR_IT (huart, USART_CLEAR_OREF);
                return;
        }

        if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
                uint8_t c = (uint8_t) (huart->Instance->DR & (uint8_t)0x00FF);

                if (u->onData) {
                        u->onData (c);
                }
                if (u->sink) {
                        u->sink->onData (char(c));
                }
        }
}
#endif
