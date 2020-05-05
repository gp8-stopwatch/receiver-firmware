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

#if defined(USE_USART1) || defined(USE_UART1)
Usart *Usart::usart1;
#endif
#if defined(USE_USART2) || defined(USE_UART2)
Usart *Usart::usart2;
#endif
#if defined(USE_USART3) || defined(USE_UART3)
Usart *Usart::usart3;
#endif
#if defined(USE_USART4) || defined(USE_UART4)
Usart *Usart::usart4;
#endif
#if defined(USE_USART5) || defined(USE_UART5)
Usart *Usart::usart5;
#endif
#if defined(USE_USART6) || defined(USE_UART6)
Usart *Usart::usart6;
#endif

/*****************************************************************************/

Usart::Usart (USART_TypeDef *instance, uint32_t baudRate) : sink (nullptr)
{
#if defined(USE_USART1)
        if (instance == USART1) {
                usart1 = this;
        }
#elif defined(USE_UART1)
        if (instance == UART1) {
                usart1 = this;
        }
#endif

#if defined(USE_USART2)
        else if (instance == USART2) {
                usart2 = this;
        }
#elif defined(USE_UART2)
        else if (instance == UART2) {
                usart2 = this;
        }
#endif

#if defined(USE_USART3)
        else if (instance == USART3) {
                usart3 = this;
        }
#elif defined(USE_UART3)
        else if (instance == UART3) {
                usart3 = this;
        }
#endif

#if defined(USE_USART4)
        else if (instance == USART4) {
                usart4 = this;
        }
#elif defined(USE_UART4)
        else if (instance == UART4) {
                usart4 = this;
        }
#endif

#if defined(USE_USART5)
        else if (instance == USART5) {
                usart5 = this;
        }
#elif defined(USE_UART5)
        else if (instance == UART5) {
                usart5 = this;
        }
#endif

#if defined(USE_USART6)
        else if (instance == USART6) {
                usart6 = this;
        }
#elif defined(USE_UART6)
        else if (instance == UART6) {
                usart6 = this;
        }
#endif

        memset (&huart, 0, sizeof (huart));
        huart.Instance = instance;
        huart.Init.BaudRate = baudRate;
        huart.Init.WordLength = UART_WORDLENGTH_8B;
        huart.Init.StopBits = UART_STOPBITS_1;
        huart.Init.Parity = UART_PARITY_NONE;
        huart.Init.Mode = UART_MODE_TX_RX;
        huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart.Init.OverSampling = UART_OVERSAMPLING_16;

        clkEnable ();

        if (HAL_UART_Init (&huart) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

Usart::~Usart () { clkDisable (); }

/*****************************************************************************/

void Usart::clkEnable (USART_TypeDef *instance)
{
#if defined(USE_USART1)
        if (instance == USART1) {
                __HAL_RCC_USART1_CLK_ENABLE ();
        }
#elif defined(USE_UART1)
        if (instance == UART1) {
                __HAL_RCC_UART1_CLK_ENABLE ();
        }
#endif

#if defined(USE_USART2)
        else if (instance == USART2) {
                __HAL_RCC_USART2_CLK_ENABLE ();
        }
#elif defined(USE_UART2)
        else if (instance == UART2) {
                __HAL_RCC_UART2_CLK_ENABLE ();
        }
#endif

#if defined(USE_USART3)
        if (instance == USART3) {
                __HAL_RCC_USART3_CLK_ENABLE ();
        }
#elif defined(USE_UART3)
        if (instance == UART3) {
                __HAL_RCC_UART3_CLK_ENABLE ();
        }
#endif

#if defined(USE_USART4)
        if (instance == USART4) {
                __HAL_RCC_USART4_CLK_ENABLE ();
        }
#elif defined(USE_UART4)
        if (instance == UART4) {
                __HAL_RCC_UART4_CLK_ENABLE ();
        }
#endif

#if defined(USE_USART5)
        if (instance == USART5) {
                __HAL_RCC_USART5_CLK_ENABLE ();
        }
#elif defined(USE_UART5)
        if (instance == UART5) {
                __HAL_RCC_UART5_CLK_ENABLE ();
        }
#endif

#if defined(USE_USART6)
        if (instance == USART6) {
                __HAL_RCC_USART6_CLK_ENABLE ();
        }
#elif defined(USE_UART6)
        if (instance == UART6) {
                __HAL_RCC_UART6_CLK_ENABLE ();
        }
#endif
}

/*****************************************************************************/

void Usart::clkDisable (USART_TypeDef *instance)
{
#if defined(USE_USART1)
        if (instance == USART1) {
                __HAL_RCC_USART1_CLK_DISABLE ();
        }
#elif defined(USE_UART1)
        if (instance == UART1) {
                __HAL_RCC_UART1_CLK_DISABLE ();
        }
#endif

#if defined(USE_USART2)
        else if (instance == USART2) {
                __HAL_RCC_USART2_CLK_DISABLE ();
        }
#elif defined(USE_UART2)
        else if (instance == UART2) {
                __HAL_RCC_UART2_CLK_DISABLE ();
        }
#endif

#if defined(USE_USART3)
        if (instance == USART3) {
                __HAL_RCC_USART3_CLK_DISABLE ();
        }
#elif defined(USE_UART3)
        if (instance == UART3) {
                __HAL_RCC_UART3_CLK_DISABLE ();
        }
#endif

#if defined(USE_USART4)
        if (instance == USART4) {
                __HAL_RCC_USART4_CLK_DISABLE ();
        }
#elif defined(USE_UART4)
        if (instance == UART4) {
                __HAL_RCC_UART4_CLK_DISABLE ();
        }
#endif

#if defined(USE_USART5)
        if (instance == USART5) {
                __HAL_RCC_USART5_CLK_DISABLE ();
        }
#elif defined(USE_UART5)
        if (instance == UART5) {
                __HAL_RCC_UART5_CLK_DISABLE ();
        }
#endif

#if defined(USE_USART6)
        if (instance == USART6) {
                __HAL_RCC_USART6_CLK_DISABLE ();
        }
#elif defined(USE_UART6)
        if (instance == UART6) {
                __HAL_RCC_UART6_CLK_DISABLE ();
        }
#endif
}

/*****************************************************************************/

void Usart::transmit (const uint8_t *str, size_t len) { HAL_UART_Transmit (&huart, const_cast<uint8_t *> (str), len, 5000); }

/*****************************************************************************/

void Usart::transmit (const char *str, size_t len) { HAL_UART_Transmit (&huart, (uint8_t *)(str), len, 5000); }

/*****************************************************************************/

void Usart::transmit (const char *str) { transmit (reinterpret_cast<uint8_t const *> (str), strlen (str)); }

/*****************************************************************************/

void Usart::startReceive ()
{
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_PEF);
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_FEF);
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_NEF);
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_OREF);
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_IDLEF);
        __HAL_USART_CLEAR_IT (&huart, USART_CLEAR_TCF);

        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        huart.Instance->CR3 |= USART_CR3_EIE;

        // Enable the UART Parity Error and Data Register not empty Interrupts
        huart.Instance->CR1 |= (USART_CR1_PEIE | USART_CR1_RXNEIE);
}

/*****************************************************************************/

void Usart::stopReceive ()
{
        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        huart.Instance->CR3 &= ~USART_CR3_EIE;

        // Enable the UART Parity Error and Data Register not empty Interrupts
        huart.Instance->CR1 &= ~(USART_CR1_PEIE | USART_CR1_RXNEIE);
}

/*****************************************************************************/

void Usart::pause () { huart.Instance->CR1 &= ~USART_CR1_RXNEIE; }

/*****************************************************************************/

void Usart::resume () { huart.Instance->CR1 |= USART_CR1_RXNEIE; }

/*****************************************************************************/

void Usart::fireOnData (Usart *u)
{
        // If it were not initialized
        if (!u) {
                return;
        }

        UART_HandleTypeDef *huart = &u->huart;

        uint32_t isrflags = READ_REG (huart->Instance->ISR);
        uint32_t cr1its = READ_REG (huart->Instance->CR1);
        uint32_t cr3its = READ_REG (huart->Instance->CR3);

        // Parity error
        if ((isrflags & USART_ISR_PE) && (cr1its & USART_CR1_PEIE)) {
                Error_Handler ();
        }

        // Framing error
        if ((isrflags & USART_ISR_FE) && (cr3its & USART_CR3_EIE)) {
                //__HAL_USART_CLEAR_IT (huart, USART_CLEAR_FEF);
                 Error_Handler ();
        }

        // Noise
        if ((isrflags & USART_ISR_NE) && (cr3its & USART_CR3_EIE)) {
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
