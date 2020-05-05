/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_SPI_NRG_H
#define LIB_MICRO_SPI_NRG_H

#include "Gpio.h"
#include "Hal.h"
#include <cstdint>

/**
 * @brief SPI master mode.
 */
class Spi {
public:
        Spi (uint8_t mode = SPI_Mode_Master, uint32_t baudRate = 1000000, uint8_t dataSize = SPI_DataSize_8b, uint8_t phase = SPI_CPHA_1Edge,
             uint8_t polarityClockSteadyState = SPI_CPOL_Low);

        /// Makes a copy which lets you to use the same SPI instance with another nss pin. Dies not initialize of course.
        Spi (Spi const &s);

        ~Spi ();

        /**
         * @brief Transmit and receive at the same time in 8 bits mode.
         * If you are not interested in received data, simply leave rxData nullptr.
         * Warning! Remember to use satNss (false) before transfer, and setNss (true) after.
         * @param txData
         * @param size
         * @param rxData Leave nullptr if not interested.
         */
        void transmit8 (uint8_t const *txData, size_t size, uint8_t *rxData = nullptr, size_t bogoDelay = 0);

        void receive8 (uint8_t *rxData, size_t size, size_t bogoDelay = 0);

        /**
         * @brief Transmit one byte, and return what has been sent back.
         * Warning! Remember to use satNss (false) before transfer, and setNss (true) after.
         */
        uint8_t transmit8 (uint8_t word);

        void setNssGpio (Gpio *n) { nssPin = n; }
        void setNss (bool b) { nssPin->set (b); }

        /**
         * Clears RX FIFO overflow. Overflow occurs in 2-way communications when you write data to DR register, but you don't read DR. Reading DR
         * frees RX FIFO.
         */
        void clearOvr ();

private:
        Gpio *nssPin;
};

#endif // SPI_H
