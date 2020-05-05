/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Gpio.h"
#include <cstdint>
#include <cstring>

/*****************************************************************************/

// uint16_t Gpio::connectedExtisMask = 0;

Gpio *Gpio::connectedExtis[16] = {
        0,
};

/*****************************************************************************/

Gpio::Gpio (uint32_t pin, uint32_t mode, FunctionalState pull, FunctionalState hipower)
{
        GPIO_InitType gpioInitStructure;
        memset (&gpioInitStructure, 0, sizeof (gpioInitStructure));
        this->pin = pin;
        gpioInitStructure.GPIO_Pin = pin;
        gpioInitStructure.GPIO_Mode = mode;
        gpioInitStructure.GPIO_Pull = pull;
        gpioInitStructure.GPIO_HighPwr = hipower;
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_GPIO, ENABLE);
        GPIO_Init (&gpioInitStructure);
}

/*****************************************************************************/

Gpio::~Gpio ()
{
        int pinNo = pinNumber (pin);
        connectedExtis[pinNo] = nullptr;
}

/*****************************************************************************/

void Gpio::set (bool on)
{
        if (on) {
                GPIO->DATS = pin;
        }
        else {
                GPIO->DATC = pin;
        }
}

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

/*****************************************************************************/

extern "C" void GPIO_Handler ()
{
        uint8_t pinNo;

        if (GPIO->MIS & GPIO_Pin_0) {
                GPIO->IC |= GPIO_Pin_0;
                pinNo = 0;
        }
        else if (GPIO->MIS & GPIO_Pin_1) {
                GPIO->IC |= GPIO_Pin_1;
                pinNo = 1;
        }
        else if (GPIO->MIS & GPIO_Pin_2) {
                GPIO->IC |= GPIO_Pin_2;
                pinNo = 2;
        }
        else if (GPIO->MIS & GPIO_Pin_3) {
                GPIO->IC |= GPIO_Pin_3;
                pinNo = 3;
        }
        else if (GPIO->MIS & GPIO_Pin_4) {
                GPIO->IC |= GPIO_Pin_4;
                pinNo = 4;
        }
        else if (GPIO->MIS & GPIO_Pin_5) {
                GPIO->IC |= GPIO_Pin_5;
                pinNo = 5;
        }
        else if (GPIO->MIS & GPIO_Pin_6) {
                GPIO->IC |= GPIO_Pin_6;
                pinNo = 6;
        }
        else if (GPIO->MIS & GPIO_Pin_7) {
                GPIO->IC |= GPIO_Pin_7;
                pinNo = 7;
        }
        else if (GPIO->MIS & GPIO_Pin_8) {
                GPIO->IC |= GPIO_Pin_8;
                pinNo = 8;
        }
        else if (GPIO->MIS & GPIO_Pin_9) {
                GPIO->IC |= GPIO_Pin_9;
                pinNo = 9;
        }
        else if (GPIO->MIS & GPIO_Pin_10) {
                GPIO->IC |= GPIO_Pin_10;
                pinNo = 10;
        }
        else if (GPIO->MIS & GPIO_Pin_11) {
                GPIO->IC |= GPIO_Pin_11;
                pinNo = 11;
        }
        else if (GPIO->MIS & GPIO_Pin_12) {
                GPIO->IC |= GPIO_Pin_12;
                pinNo = 12;
        }
        else if (GPIO->MIS & GPIO_Pin_13) {
                GPIO->IC |= GPIO_Pin_13;
                pinNo = 13;
        }
        else if (GPIO->MIS & GPIO_Pin_14) {
                GPIO->IC |= GPIO_Pin_14;
                pinNo = 14;
        }
        else if (GPIO->MIS & GPIO_Pin_15) {
                GPIO->IC |= GPIO_Pin_15;
                pinNo = 15;
        }

        // TODO new versions of BlueNRG-2 have more pins (26 i think).
        Gpio::connectedExtis[pinNo]->onToggle ();
}
