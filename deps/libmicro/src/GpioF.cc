/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Gpio.h"
#include <cstring>

/*****************************************************************************/

// uint16_t Gpio::connectedExtisMask = 0;

Gpio *Gpio::connectedExtis[16] = {
        0,
};

/*****************************************************************************/

Gpio::Gpio (GPIO_TypeDef *port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate)
{
        GPIO_InitTypeDef gpioInitStructure;
        memset (&gpioInitStructure, 0, sizeof (gpioInitStructure));
        this->port = port;
        this->pin = pin;
        gpioInitStructure.Pin = pin;
        gpioInitStructure.Mode = mode;
        gpioInitStructure.Pull = pull;
        gpioInitStructure.Speed = speed;
        gpioInitStructure.Alternate = alternate;
        clkEnable ();
        HAL_GPIO_Init (port, &gpioInitStructure);
}

/*****************************************************************************/

Gpio::~Gpio ()
{
        clkDisable ();
        HAL_GPIO_DeInit (port, pin);
        int pinNo = pinNumber (pin);
        connectedExtis[pinNo] = nullptr;
}

/*****************************************************************************/

void Gpio::clkEnable (GPIO_TypeDef *port)
{
        if (port == GPIOA) {
                __HAL_RCC_GPIOA_CLK_ENABLE ();
        }
        else if (port == GPIOB) {
                __HAL_RCC_GPIOB_CLK_ENABLE ();
        }
        else if (port == GPIOC) {
                __HAL_RCC_GPIOC_CLK_ENABLE ();
        }
#if !defined(STM32F042x6)
        else if (port == GPIOD) {
                __HAL_RCC_GPIOD_CLK_ENABLE ();
        }
        else if (port == GPIOE) {
                __HAL_RCC_GPIOE_CLK_ENABLE ();
        }
#endif
        else if (port == GPIOF) {
                __HAL_RCC_GPIOF_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void Gpio::clkDisable (GPIO_TypeDef *port)
{
        if (port == GPIOA) {
                __HAL_RCC_GPIOA_CLK_DISABLE ();
        }
        else if (port == GPIOB) {
                __HAL_RCC_GPIOB_CLK_DISABLE ();
        }
        else if (port == GPIOC) {
                __HAL_RCC_GPIOC_CLK_DISABLE ();
        }
#if !defined(STM32F042x6)
        else if (port == GPIOD) {
                __HAL_RCC_GPIOD_CLK_DISABLE ();
        }
        else if (port == GPIOE) {
                __HAL_RCC_GPIOE_CLK_DISABLE ();
        }
#endif
        else if (port == GPIOF) {
                __HAL_RCC_GPIOF_CLK_DISABLE ();
        }
}

/*****************************************************************************/

void Gpio::set (bool on)
{
        if (on) {
                port->BSRR = pin;
        }
        else {
                //                port->BRR = gpioInitStructure.Pin;
                port->BSRR = pin << 16;
        }
}

/*****************************************************************************/

bool Gpio::get () const { return port->IDR & pin; }

/*****************************************************************************/

void Gpio::setOnToggle (std::function<void(void)> const &t)
{
        int pinNo = pinNumber (pin);

        if (connectedExtis[pinNo]) {
                /*
                 * You cannot set a callback for this GPIO, because the pin it is configured for
                 * is already used as EXTI source. You can have only one Gpio  pin number configured
                 * as EXTI for example if you configure PA0 as an EXTI you cannot use any of PB0,
                 * PC0 and so on as EXTIS. See this conversation for example :
                 * https://stackoverflow.com/questions/23043465/find-specific-pin-on-which-a-interrupt-occured
                 */
                Error_Handler ();
        }

        connectedExtis[pinNo] = this;
        onToggle = t;
}

/*****************************************************************************/

int Gpio::pinNumber (uint32_t pinDef)
{
        int res = 0;
        while (pinDef >>= 1) {
                ++res;
        }

        return res;
}
