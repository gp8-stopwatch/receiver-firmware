/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_SPI_F_H
#define LIB_MICRO_SPI_F_H

#include "Gpio.h"
#include "Hal.h"
#include "ISpiCallback.h"

/**
 * @brief SPI master mode.
 */
class Spi {
public:
        enum { POLLING_TIMEOUT_MS = 500 };

        // Commented the default, to prevent from unnoticed nasty errors. Those parameters should have its own enum types
        Spi (SPI_TypeDef *spi, uint32_t prescaler /*= SPI_BAUDRATEPRESCALER_32*/, uint32_t mode = SPI_MODE_MASTER,
             uint32_t dataSize = SPI_DATASIZE_8BIT, uint32_t phase = SPI_PHASE_1EDGE, uint32_t polarityClockSteadyState = SPI_POLARITY_LOW,
             uint32_t nssMode = SPI_NSS_SOFT);

        ~Spi () { clkDisable (); }

        /*****************************************************************************/
        /* Polling                                                                   */
        /*****************************************************************************/

        /**
         * @brief Transmit and receive at the same time in 8 bits mode.
         * If you are not interested in received data, simply leave rxData nullptr.
         * Warning! Remember to use satNss (false) before transfer, and setNss (true) after.
         * @param txData
         * @param size
         * @param rxData Leave nullptr if not interested.
         */
        void transmit8 (uint8_t const *txData, uint16_t size, uint8_t *rxData = nullptr, size_t bogoDelay = 0, bool dataPacking = true);
        void receive8 (uint8_t *rxData, uint16_t size, size_t bogoDelay = 0, bool dataPacking = true);

        /**
         * @brief Transmit one byte, and return what has been sent back.
         * Warning! Remember to use setNss (false) before transfer, and setNss (true) after.
         */
        uint8_t transmit8 (uint8_t word);

        /// Receive-only counterpart of single-byte transmit8.
        uint8_t receive8 ();

        /*****************************************************************************/
        /* Interrupts                                                                */
        /*****************************************************************************/

        // TODO Make (interafce) and SpiSlave class!
        // Slave methods
        //        uint8_t receive8NonBlocking (); /// Non blocking method
        //        void transmit8nr (uint8_t word);
        //        void transmit8nrNb (uint8_t word);

        void transmit8nr (uint8_t const *txData, uint16_t size, uint8_t *rxData = nullptr);
        void receive8nb (uint8_t *rxData, uint16_t size);

        /// Inetrrupts masking parameter
        enum Interrupts { NO_INTERRUPTS = 0x00, RXNE_INTERRUPT = 0x01, TXE_INTERRUPT = 0x02, ERR_INTERRUPT = 0x04, ALL_INTERRUPTS = 0x07 };

        /// Turns interrupts on and off.
        void interrupts (uint8_t imask);

        void setCallback (ISpiCallback *c) { callback = c; }

        /*****************************************************************************/

        /**
         * @brief clkEnable Runs __HAL_RCC_SPIx_CLK_ENABLE for a SPIx.
         * @param gpiox The port.
         */
        static void clkEnable (SPI_HandleTypeDef *spiX);

        /**
         * @brief clkEnable Runs __HAL_RCC_SPIx_CLK_ENABLE for a SPIx.
         * @param gpiox The port.
         */
        static void clkDisable (SPI_HandleTypeDef *spiX);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (&spiHandle); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (&spiHandle); }

        void setNssGpio (Gpio *n) { nssPin = n; }
        void setNss (bool b) { nssPin->set (b); }

        /**
         * Clears RX FIFO overflow. Overflow occurs in 2-way communications when you write data to DR register, but you don't read DR. Reading DR
         * frees RX FIFO.
         */
        void clearOvr ();

        // private:
        void onTxEmpty ();
        void onRxNotEmpty ();
        size_t txRemainig;
        size_t rxRemainig, rxRemaining0;
        uint8_t const *txData;
        uint8_t *rxData;
        uint8_t *rxData0;

        // private:
        FRIEND_ALL_SPI_IRQS
        SPI_HandleTypeDef spiHandle;
        SPI_TypeDef *spi;
        Gpio *nssPin;
        static Spi *spi1;
        static Spi *spi2;
        ISpiCallback *callback;
};

#endif // SPI_H
