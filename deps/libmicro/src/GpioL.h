/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_GPIO_L_H
#define LIB_MICRO_GPIO_L_H

#include <Hal.h>
#include <functional>

/**
 * @brief GPIO port adapter class. RAII.
 */
class Gpio {
public:
        /// Initializes, and turns the clock on.
        Gpio (GPIO_TypeDef *port, uint32_t pin, uint32_t mode = GPIO_MODE_OUTPUT_PP, uint32_t pull = GPIO_NOPULL,
              uint32_t speed = GPIO_SPEED_FREQ_HIGH, uint32_t alternate = 0);

        ~Gpio ();

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkEnable (GPIO_TypeDef *port);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkDisable (GPIO_TypeDef *port);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (port); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (port); }

        /**
         * @brief Turns on or off.
         * @param on
         */
        void set (bool on);
        bool operator= (bool on)
        {
                set (on);
                return on;
        }

        bool get () const { return port->IDR & pin; }
        operator bool () const { return get (); }

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

        GPIO_TypeDef *getPort () { return port; }

private:
        FRIEND_ALL_GPIO_IRQS

        uint32_t pin;
        GPIO_TypeDef *port;
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
