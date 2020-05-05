/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Debug.h"
#include "ErrorHandler.h"
#include "Spi.h"
#include "Timer.h"
#include <cstring>

/*****************************************************************************/

Spi *Spi::spi1;
Spi *Spi::spi2;

/*****************************************************************************/

Spi::Spi (SPI_TypeDef *spi, uint32_t prescaler, uint32_t mode, uint32_t dataSize, uint32_t phase, uint32_t polarityClockSteadyState,
          uint32_t nssMode)
    : txRemainig (0),
      rxRemainig (0),
      rxRemaining0 (0),
      txData (nullptr),
      rxData (nullptr),
      rxData0 (nullptr),
      spi (spi),
      nssPin (nullptr),
      callback (nullptr)
{
        if (spi == SPI1) {
                spi1 = this;
        }
        else if (spi == SPI2) {
                spi2 = this;
        }

        memset (&spiHandle, 0, sizeof (spiHandle));
        spiHandle.Instance = spi;
        spiHandle.Init.BaudRatePrescaler = prescaler;
        spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
        spiHandle.Init.CLKPhase = phase;
        spiHandle.Init.CLKPolarity = polarityClockSteadyState;
        spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        spiHandle.Init.CRCPolynomial = 7;
        spiHandle.Init.DataSize = dataSize;
        spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
        spiHandle.Init.NSS = nssMode;
#ifdef LIB_MICRO_STM32F0
        spiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif
        spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
        spiHandle.Init.Mode = mode;

        clkEnable ();
        HAL_SPI_Init (&spiHandle);

        // Check if the SPI is already enabled
        if ((spiHandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

#ifdef LIB_MICRO_STM32F0
        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
#endif
}

/*****************************************************************************/

void Spi::transmit8 (uint8_t const *txData, uint16_t size, uint8_t *rxData, size_t bogoDelay, bool dataPacking)
{
        size_t txRemainig = size;
        size_t rxRemainig = (!rxData) ? (0) : (size);

#ifndef LIB_MICRO_STM32F4
        if (dataPacking && rxRemainig > 1) {
                // set fiforxthreshold according the reception data length: 16bit
                CLEAR_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
        else {
                // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
#endif

        bool txAllowed = true;

        if ((spi->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        Timer timeout;
        timeout.start (POLLING_TIMEOUT_MS);

        // Based on HAL code.
        while ((txRemainig || rxRemainig) && !timeout.isExpired ()) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && txRemainig && (spi->SR & SPI_FLAG_TXE)) {

#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && txRemainig > 1) {
                                spi->DR = *((uint16_t *)txData);
                                txData += sizeof (uint16_t);
                                txRemainig -= 2;
                        }
                        else {
#endif
                                *(__IO uint8_t *)&spi->DR = (*txData++);
                                --txRemainig;
#ifndef LIB_MICRO_STM32F4
                        }
#endif
                        if (rxRemainig) {
                                // Next Data is a reception (Rx). Tx not allowed
                                txAllowed = false;
                        }
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (rxRemainig && (spi->SR & SPI_FLAG_RXNE)) {

#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && rxRemainig > 1) {
                                *((uint16_t *)rxData) = spi->DR;
                                rxData += sizeof (uint16_t);
                                rxRemainig -= 2;

                                if (rxRemainig <= 1) {
                                        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                                        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
                                }
                        }
                        else {
#endif
                                (*(uint8_t *)rxData++) = *(__IO uint8_t *)&spi->DR;
                                --rxRemainig;
#ifndef LIB_MICRO_STM32F4
                        }
#endif
                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }

        // In 2way mode (I don't use 1way modes), when we haven't read, the OVR will  occur. So we clear it.
        if (!rxData) {
                clearOvr ();
        }

        while (spi->SR & SPI_FLAG_BSY)
                ;
}

/*****************************************************************************/

void Spi::receive8 (uint8_t *rxData, uint16_t size, size_t bogoDelay, bool dataPacking)
{
        size_t rxRemainig = (!rxData) ? (0) : (size);

#ifndef LIB_MICRO_STM32F4
        if (dataPacking && rxRemainig > 1) {
                // set fiforxthreshold according the reception data length: 16bit
                CLEAR_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
        else {
                // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
#endif

        bool txAllowed = true;

        if ((spiHandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        // Based on HAL code.
        while (rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && (spi->SR & SPI_FLAG_TXE)) {
#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && rxRemainig > 1) {
                                spi->DR = 0xffff;
                        }
                        else {
#endif
                                *(__IO uint8_t *)&spi->DR = 0xff;
#ifndef LIB_MICRO_STM32F4
                        }
#endif
                        // Next Data is a reception (Rx). Tx not allowed
                        txAllowed = false;
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (spi->SR & SPI_FLAG_RXNE) {

#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && rxRemainig > 1) {
                                *((uint16_t *)rxData) = spi->DR;
                                rxData += sizeof (uint16_t);
                                rxRemainig -= 2;

                                if (rxRemainig <= 1) {
                                        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                                        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
                                }
                        }
                        else {
#endif
                                (*(uint8_t *)rxData++) = *(__IO uint8_t *)&spi->DR;
                                --rxRemainig;
#ifndef LIB_MICRO_STM32F4
                        }
#endif

                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }

        // Avoids errors at the end
        while (spi->SR & SPI_FLAG_BSY)
                ;
}

/*****************************************************************************/

uint8_t Spi::transmit8 (uint8_t word)
{
        // Doesn't work without it
        if ((spi->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        // Flush input buffer.
        uint8_t ret = *(__IO uint8_t *)&spi->DR;

        // Wait for tx buffer to be empty, which means we can write to DR.
        while (!(spi->SR & SPI_FLAG_TXE))
                ;

        *(__IO uint8_t *)&spi->DR = word;

#ifndef LIB_MICRO_STM32F4
        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
#endif

        while (!(spi->SR & SPI_FLAG_RXNE))
                ;

        ret = *(__IO uint8_t *)&spi->DR;

        while (spi->SR & SPI_FLAG_BSY)
                ;

        return ret;
}

/*****************************************************************************/

uint8_t Spi::receive8 ()
{
        // Doesn't work without it
        if ((spi->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

#ifndef LIB_MICRO_STM32F4
        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
#endif

        while (!(spi->SR & SPI_FLAG_RXNE))
                ;

        uint8_t ret = *(__IO uint8_t *)&spi->DR;

        while (spi->SR & SPI_FLAG_BSY)
                ;

        return ret;
}

/*****************************************************************************/

// uint8_t Spi::receive8NonBlocking () { return *(__IO uint8_t *)&spiHandle.Instance->DR; }

/*---------------------------------------------------------------------------*/

// void Spi::transmit8nr (uint8_t word)
//{
//        // Wait for tx buffer to be empty
//        while (!(spiHandle.Instance->SR & SPI_FLAG_TXE))
//                ;

//        *(__IO uint8_t *)&spiHandle.Instance->DR = word;
//}

///*---------------------------------------------------------------------------*/

// void Spi::transmit8nrNb (uint8_t word) { *(__IO uint8_t *)&spiHandle.Instance->DR = word; }

/*---------------------------------------------------------------------------*/

void Spi::transmit8nr (uint8_t const *txData, uint16_t size, uint8_t *rxData)
{
        if (!size) {
                return;
        }

        txRemainig = size;
        this->txData = txData;
        uint32_t interrupts = SPI_IT_TXE;

        if (rxData) {
                rxData0 = this->rxData = rxData;
                rxRemaining0 = size;
                rxRemainig = size;
                interrupts |= SPI_IT_RXNE;
        }

        //        Debug::singleton ()->print ("T+");
        __HAL_SPI_ENABLE_IT (&spiHandle, interrupts);
}

/*---------------------------------------------------------------------------*/

void Spi::onTxEmpty ()
{
        if (!txRemainig) {
                //                Debug::singleton ()->print ("T-");
                __HAL_SPI_DISABLE_IT (&spiHandle, (SPI_IT_TXE));
                callback->onTxComplete ();
                return;
        }

#ifndef LIB_MICRO_STM32F4
//        if (txRemainig > 1) {
//                spi->DR = *((uint16_t *)txData);
//                txData += sizeof (uint16_t);
//                txRemainig -= 2;
//        }
//        else {
#endif
        *(__IO uint8_t *)&spi->DR = (*txData++);
        --txRemainig;
#ifndef LIB_MICRO_STM32F4
//        }
#endif
}

/*---------------------------------------------------------------------------*/

void Spi::receive8nb (uint8_t *rxData, uint16_t size)
{
        rxData0 = this->rxData = rxData;
        rxRemaining0 = size;
        rxRemainig = size;
        // Debug::singleton ()->print ("R+");
        __HAL_SPI_ENABLE_IT (&spiHandle, (SPI_IT_RXNE));
}

/*---------------------------------------------------------------------------*/

void Spi::onRxNotEmpty ()
{
        if (!rxRemainig) {
                uint8_t b = *(__IO uint8_t *)&spi->DR;
                (void)b;
                return;
        }

        *rxData++ = *(__IO uint8_t *)&spi->DR;
        --rxRemainig;

        if (!rxRemainig) {
                // Debug::singleton ()->print ("R-");
                __HAL_SPI_DISABLE_IT (&spiHandle, (SPI_IT_RXNE));
                callback->onRxComplete (rxData0, rxRemaining0);
        }
}

/*****************************************************************************/

void Spi::clkEnable (SPI_HandleTypeDef *spiX)
{
        if (spiX->Instance == SPI1) {
                __HAL_RCC_SPI1_CLK_ENABLE ();
        }
        else if (spiX->Instance == SPI2) {
                __HAL_RCC_SPI2_CLK_ENABLE ();
        }
#if !defined(LIB_MICRO_STM32F0)
        else if (spiX->Instance == SPI3) {
                __HAL_RCC_SPI3_CLK_ENABLE ();
        }
#endif

        __HAL_SPI_ENABLE (spiX);
}

/*****************************************************************************/

void Spi::clkDisable (SPI_HandleTypeDef *spiX)
{
        if (spiX->Instance == SPI1) {
                __HAL_RCC_SPI1_CLK_DISABLE ();
        }
        else if (spiX->Instance == SPI2) {
                __HAL_RCC_SPI2_CLK_DISABLE ();
        }
#if !defined(LIB_MICRO_STM32F0)
        else if (spiX->Instance == SPI3) {
                __HAL_RCC_SPI3_CLK_DISABLE ();
        }
#endif
        __HAL_SPI_DISABLE (spiX);
}

/*****************************************************************************/

void Spi::clearOvr ()
{
#ifndef LIB_MICRO_STM32F4
        SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
#endif
        volatile uint16_t tmp;

        while (spiHandle.Instance->SR & SPI_FLAG_RXNE) {
                tmp = *(__IO uint8_t *)&spiHandle.Instance->DR;
        }

        tmp = spiHandle.Instance->SR;
        (void)tmp;
}

/*****************************************************************************/

void Spi::interrupts (uint8_t imask)
{
        if (imask & RXNE_INTERRUPT) {
#ifndef LIB_MICRO_STM32F4
                // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
#endif

                __HAL_SPI_ENABLE_IT (&spiHandle, (SPI_IT_RXNE));
        }
        else {
                __HAL_SPI_DISABLE_IT (&spiHandle, (SPI_IT_RXNE));
        }

        if (imask & TXE_INTERRUPT) {
                __HAL_SPI_ENABLE_IT (&spiHandle, (SPI_IT_TXE));
        }
        else {
                __HAL_SPI_DISABLE_IT (&spiHandle, (SPI_IT_TXE));
        }

        if (imask & ERR_INTERRUPT) {
                __HAL_SPI_ENABLE_IT (&spiHandle, (SPI_IT_ERR));
        }
        else {
                __HAL_SPI_DISABLE_IT (&spiHandle, (SPI_IT_ERR));
        }
}
