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
#include "II2cCallback.h"
#include <cstdint>
#include <cstdlib>

extern "C" void I2C1_IRQHandler ();
extern "C" void I2C2_IRQHandler ();

/**
 * @brief The I2c class
 */
class I2c {
public:
        I2c (I2C_TypeDef *hi2c);

        enum { DEFAULT_TIMEOUT = 1000, RX_BUFFER_SIZE = 16 };
        void read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void write (uint8_t devAddr, uint8_t regAddr, uint8_t const *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        bool slaveWrite (const uint8_t *data, size_t length);
        void setCallback (II2cCallback *c) { callback = c; }

        /**
         * Resets the peripheral. See reference manual 26.4.6 Software reset.
         */
        void reset ();
        void flushTx ();

        void clkEnable ();
        void clkDisable ();

private:
        friend void I2C1_IRQHandler ();
        friend void I2C2_IRQHandler ();
        void slaveIrq (I2c *i2c);

        enum State { SLAVE_ADDR_LISTEN, SLAVE_BYTE_RX, SLAVE_BYTE_TX };

        I2C_HandleTypeDef i2cHandle;
        II2cCallback *callback;
        State state;
        static I2c *i2c1;
        static I2c *i2c2;

        /// Addres by witch we were called as a slave.
        uint16_t currentAddress;
        mutable const uint8_t *txBuffer, *txPointer;
        size_t txToSend, txRemaining;
        uint8_t rxBuffer[RX_BUFFER_SIZE], *rxPointer;
        size_t rxReceived;
};

#endif // I2C_H
