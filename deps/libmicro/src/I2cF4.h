/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_I2C_H
#define LIB_MICRO_I2C_H

#include "Hal.h"
#include <cstdint>
#include <cstdlib>

/**
 * @brief The I2c class
 */
class I2c {
public:
        I2c (I2C_TypeDef *hi2c);

        enum { DEFAULT_TIMEOUT = 1000, RX_BUFFER_SIZE = 16 };
        void read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void write (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        /**
         * Resets the peripheral. See reference manual 26.4.6 Software reset.
         */
        void reset ();

        void clkEnable ();
        void clkDisable ();

private:
        I2C_HandleTypeDef i2cHandle;
        static I2c *i2c1;
        static I2c *i2c2;
};

#endif // I2C_H
