/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#define DEBUG_UART USART1
#define DEBUG_PORT GPIOA
#define DEBUG_PINS (GPIO_PIN_9 | GPIO_PIN_10)
#define DEBUG_ALTERNATE GPIO_AF1_USART1

#define CAN_PORT_1 GPIOB
#define CAN_PIN_1 GPIO_PIN_8
#define CAN_PORT_2 GPIOB
#define CAN_PIN_2 GPIO_PIN_9
#define CAN_ALTERNATE GPIO_AF4_CAN

#define IR_PORT GPIOA
#define IR_PINS GPIO_PIN_8
#define BUTTON_AND_IR_IRQn EXTI4_15_IRQn

#define TEST_TRIGGER_PORT GPIOB
#define TEST_TRIGGER_PINS GPIO_PIN_3
#define TEST_TRIGGER_IRQn EXTI2_3_IRQn