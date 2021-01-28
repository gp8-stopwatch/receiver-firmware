/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#define DEBUG_UART USART2
#define DEBUG_PORT GPIOA
#define DEBUG_PINS (GPIO_PIN_2 | GPIO_PIN_3)
#define DEBUG_ALTERNATE GPIO_AF1_USART2

#define CAN_PORT_1 GPIOB
#define CAN_PIN_1 GPIO_PIN_8
#define CAN_PORT_2 GPIOA
#define CAN_PIN_2 GPIO_PIN_12
#define CAN_ALTERNATE GPIO_AF4_CAN

#define IR_PORT GPIOA
#define IR_PINS GPIO_PIN_5
#define IR_IRQn EXTI4_15_IRQn

// Test trigger
#define EXT_TRIGGER_INPUT_PORT GPIOA
#define EXT_TRIGGER_INPUT_PINS GPIO_PIN_0
#define EXT_TRIGGER_INPUT_IRQn EXTI0_1_IRQn

#define EXT_TRIGGER_OUTPUT_PORT GPIOA
#define EXT_TRIGGER_OUTPUT_PINS GPIO_PIN_1

#define EXT_TRIGGER_OUT_ENABLE_PORT GPIOA
#define EXT_TRIGGER_OUT_ENABLE_PINS GPIO_PIN_4

#define BUTTON_PORT GPIOB
#define BUTTON_PINS GPIO_PIN_15
#define BUTTON_IRQn EXTI4_15_IRQn
