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

Usart::Usart (uint32_t baudRate) : sink (nullptr) {}

/*****************************************************************************/

Usart::~Usart ()
{ /*clkDisable ();*/
}

/*****************************************************************************/

void Usart::transmit (const uint8_t *str, size_t len)
{
        // received.push_back ();
        // std::cerr << str << std::endl;
}

/*****************************************************************************/

void Usart::transmit (const char *str, size_t len) { /*HAL_UART_Transmit (&huart, (uint8_t *)(str), len, 5000);*/ }

/*****************************************************************************/

void Usart::transmit (const char *str) { transmit (reinterpret_cast<uint8_t const *> (str), strlen (str)); }

/*****************************************************************************/

void Usart::startReceive () { receiving = true; }

/*****************************************************************************/

void Usart::stopReceive () { receiving = false; }

/*****************************************************************************/

void Usart::pause () { stopReceive (); }

/*****************************************************************************/

void Usart::resume () { startReceive (); /*?*/ }

/*****************************************************************************/

void Usart::fireOnData (Usart *u)
{
//        // If it were not initialized
//        if (!u) {
//                return;
//        }

//        UART_HandleTypeDef *huart = &u->huart;

//        uint32_t isrflags = READ_REG (huart->Instance->ISR);
//        uint32_t cr1its = READ_REG (huart->Instance->CR1);

//        if (isrflags & uint32_t (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE)) {
//                __HAL_USART_CLEAR_IT (huart, USART_ISR_PE);
//                __HAL_USART_CLEAR_IT (huart, USART_ISR_FE);
//                __HAL_USART_CLEAR_IT (huart, USART_ISR_NE);

//                if (u->sink) {
//                        u->sink->onError (isrflags);
//                }

//                return;
//        }

//        // TODO ORE (OverRun Error) is silently discarded
//        if (isrflags & uint32_t (USART_ISR_ORE)) {
//                __HAL_USART_CLEAR_IT (huart, USART_CLEAR_OREF);

//                if (u->sink) {
//                        u->sink->onError (uint32_t (USART_ISR_ORE));
//                }
//                return;
//        }

//        if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
//                uint8_t c = (uint8_t) (huart->Instance->RDR & (uint8_t)0x00FF);

//                if (u->onData) {
//                        u->onData (c);
//                }
//                if (u->sink) {
//                        u->sink->onData (char(c));
//                }
//        }
}
