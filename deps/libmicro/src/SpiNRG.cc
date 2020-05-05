/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Spi.h"
#include <cstring>

/*****************************************************************************/

Spi::Spi (uint8_t mode, uint32_t baudRate, uint8_t dataSize, uint8_t phase, uint8_t polarityClockSteadyState) : nssPin (nullptr)
{
        SPI_InitType spiInitStructure;
        memset (&spiInitStructure, 0, sizeof (spiInitStructure));

        SPI_StructInit (&spiInitStructure);
        spiInitStructure.SPI_Mode = mode;
        spiInitStructure.SPI_DataSize = dataSize;
        spiInitStructure.SPI_CPOL = polarityClockSteadyState;
        spiInitStructure.SPI_CPHA = phase;
        spiInitStructure.SPI_BaudRate = baudRate;

        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_SPI, ENABLE);
        SPI_Init (&spiInitStructure);

        SPI_ClearTXFIFO ();
        SPI_ClearRXFIFO ();
        SPI_SetDummyCharacter (0xFF);
        SPI_SetMasterCommunicationMode (SPI_FULL_DUPLEX_MODE);

        // Enable SPI functionality
        SPI_Cmd (ENABLE);
}

/*****************************************************************************/

Spi::Spi (Spi const &s) { this->nssPin = s.nssPin; }

/*****************************************************************************/

Spi::~Spi ()
{
        SPI_Cmd (DISABLE);
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_SPI, DISABLE);
}

/*****************************************************************************/

void Spi::transmit8 (uint8_t const *txData, size_t size, uint8_t *rxData, size_t bogoDelay)
{
        SPI_ClearTXFIFO ();
        SPI_ClearRXFIFO ();

        size_t txRemaining = size;
        size_t rxRemainig = (!rxData) ? (0) : (size);

        bool txAllowed = true;

        while (txRemaining || rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && txRemaining && (SPI->SR & SPI_FLAG_TFE)) {
                        *reinterpret_cast<__IO uint8_t *> (&SPI->DR) = *txData++;
                        --txRemaining;

                        // Next Data is a reception (Rx). Tx not allowed
                        if (rxRemainig) {
                                txAllowed = false;
                        }
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (rxRemainig && (SPI->SR & SPI_FLAG_RNE)) {

                        (*rxData++) = *reinterpret_cast<__IO uint8_t *> (&SPI->DR);
                        --rxRemainig;

                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }

        while (SPI->SR & SPI_FLAG_BSY)
                ;
}

/*****************************************************************************/

void Spi::receive8 (uint8_t *rxData, size_t size, size_t bogoDelay)
{
        SPI_ClearTXFIFO ();
        SPI_ClearRXFIFO ();

        size_t rxRemainig = (!rxData) ? (0) : (size);

        bool txAllowed = true;

        // Based on HAL code.
        while (rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && (SPI->SR & SPI_FLAG_TFE)) {
                        *reinterpret_cast<__IO uint8_t *> (&SPI->DR) = 0xff;
                        // Next Data is a reception (Rx). Tx not allowed
                        txAllowed = false;
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (SPI->SR & SPI_FLAG_RNE) {

                        (*rxData++) = *reinterpret_cast<__IO uint8_t *> (&SPI->DR);
                        --rxRemainig;

                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }

        while (SPI->SR & SPI_FLAG_BSY)
                ;
}

/*****************************************************************************/

uint8_t Spi::transmit8 (uint8_t word)
{
        SPI_ClearTXFIFO ();
        SPI_ClearRXFIFO ();

        while (!(SPI->SR & SPI_FLAG_TFE))
                ;

        SPI->DR = word;

        while (!(SPI->SR & SPI_FLAG_RNE))
                ;

        uint8_t ret = *reinterpret_cast<__IO uint8_t *> (&SPI->DR);

        while (SPI->SR & SPI_FLAG_BSY)
                ;

        return ret;
}

/*****************************************************************************/

void Spi::clearOvr ()
{
#if 0
#ifndef LIB_MICRO_STM32F4
        SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
#endif
        volatile uint16_t tmp;

        while (spiHandle.Instance->SR & SPI_FLAG_RXNE) {
                tmp = *(__IO uint8_t *)&spiHandle.Instance->DR;
        }

        tmp = spiHandle.Instance->SR;
        (void)tmp;
#endif
}
