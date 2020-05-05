/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_GPIO_NRG_H
#define LIB_MICRO_GPIO_NRG_H

#include <Hal.h>
#include <cstdint>
#include <functional>

extern "C" void GPIO_Handler ();

/**
 * @brief GPIO port adapter class. RAII.
 */
class Gpio {
public:
        /// Initializes, and turns the clock on.
        Gpio (uint32_t pin, uint32_t mode = GPIO_Output, FunctionalState pull = DISABLE, FunctionalState hipower = DISABLE);

        ~Gpio ();

        /**
         * @brief Turns on or off.
         * @param on
         */
        void set (bool on);

        /**
         * @brief Sets the callback. You must configure this Gpio in EXTI mode for this to work
         * i.e. use GPIO_MODE_IT_RISING or simmilar. Warning this callback is run from ISR!
         * @param t Callback. A function, a method or lambda function.
         */
        void setOnToggle (std::function<void(void)> const &t);

        /**
         * Helper function, which tells which pin number is it based on the GPIO_PIN_x macro parameter.
         */
        static int pinNumber (uint32_t pinDef);

        GPIO_Type *getPort () { return GPIO; }

private:
        friend void GPIO_Handler ();

        uint32_t pin;
        std::function<void(void)> onToggle;

        /**
         * Stores which Gpio object is servicing which EXTI line.
         * Also prevents from setting the onToggle callback multiple times for
         * single pin. If you set onToggle for PA0, it will prevent you from setiing
         * this callback for any other 0 pin (PB0, PC0 etc).
         */
        static Gpio *connectedExtis[16];
};

#endif // GPIO_H
