/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef HALSTM32L_H
#define HALSTM32L_H

#include <stm32l4xx.h>
#include <stm32l4xx_hal.h>
#include <stm32l4xx_hal_i2c_ex.h>

#ifdef __cplusplus
extern "C" {
#endif

void EXTI0_IRQHandler ();
void EXTI1_IRQHandler ();
void EXTI2_IRQHandler ();
void EXTI3_IRQHandler ();
void EXTI4_IRQHandler ();
void EXTI9_5_IRQHandler ();
void EXTI15_10_IRQHandler ();

void USART1_IRQHandler ();
void USART2_IRQHandler ();
void USART3_IRQHandler ();
void UART4_IRQHandler ();
void UART5_IRQHandler ();
void USART6_IRQHandler ();

void SPI1_IRQHandler ();
void SPI2_IRQHandler ();
void SPI3_IRQHandler ();

#define FRIEND_ALL_GPIO_IRQS                                                                                                                    \
        friend void EXTI0_IRQHandler ();                                                                                                        \
        friend void EXTI1_IRQHandler ();                                                                                                        \
        friend void EXTI2_IRQHandler ();                                                                                                        \
        friend void EXTI3_IRQHandler ();                                                                                                        \
        friend void EXTI4_IRQHandler ();                                                                                                        \
        friend void EXTI9_5_IRQHandler ();                                                                                                      \
        friend void EXTI15_10_IRQHandler ();

#define FRIEND_ALL_USART_IRQS                                                                                                                   \
        friend void USART1_IRQHandler ();                                                                                                       \
        friend void USART2_IRQHandler ();                                                                                                       \
        friend void USART3_IRQHandler ();                                                                                                       \
        friend void UART4_IRQHandler ();                                                                                                        \
        friend void UART5_IRQHandler ();                                                                                                        \
        friend void USART6_IRQHandler ();

#define FRIEND_ALL_SPI_IRQS                                                                                                                     \
        friend void SPI1_IRQHandler ();                                                                                                         \
        friend void SPI2_IRQHandler ();                                                                                                         \
        friend void SPI3_IRQHandler ();

#ifdef __cplusplus
}
#endif

#endif // HALSTM32F0_H
