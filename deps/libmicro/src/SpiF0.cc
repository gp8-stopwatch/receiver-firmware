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

void spiIRQHandler (Spi *spi);
extern "C" void SPI1_IRQHandler () { spiIRQHandler (Spi::spi1); }
extern "C" void SPI2_IRQHandler () { spiIRQHandler (Spi::spi2); }

/*****************************************************************************/

void spiIRQHandler (Spi *spi)
{
        SPI_HandleTypeDef *hspi = &spi->spiHandle;
        uint32_t itsource = hspi->Instance->CR2;
        uint32_t itflag = hspi->Instance->SR;

        if ((itflag & SPI_FLAG_RXNE) && (itsource & SPI_IT_RXNE) && !(itflag & SPI_FLAG_OVR)) {
                spi->onRxNotEmpty ();
                return;
        }

        if ((itflag & SPI_FLAG_TXE) && (itsource & SPI_IT_TXE)) {
                spi->onTxEmpty ();
                return;
        }

        uint32_t errorCode = 0;

        // SPI Overrun error
        if (itflag & SPI_FLAG_OVR) {
                __HAL_SPI_CLEAR_OVRFLAG (hspi);

                if (spi->rxRemainig) {
                        errorCode |= HAL_SPI_ERROR_OVR;
                }
        }

        // SPI in Error Treatment
        if (itsource & SPI_IT_ERR) {

                // SPI Mode Fault error
                if ((itflag & SPI_FLAG_MODF) != RESET) {
                        errorCode |= HAL_SPI_ERROR_MODF;
                        __HAL_SPI_CLEAR_MODFFLAG (hspi);
                }

                // SPI Frame error
                if ((itflag & SPI_FLAG_FRE) != RESET) {
                        errorCode |= HAL_SPI_ERROR_FRE;
                        __HAL_SPI_CLEAR_FREFLAG (hspi);
                }

                // SPI Frame error
                if ((itflag & SPI_FLAG_CRCERR) != RESET) {
                        errorCode |= HAL_SPI_ERROR_CRC;
                        __HAL_SPI_CLEAR_FREFLAG (hspi);
                }

                if (errorCode) {
                        // Disable all interrupts
                        __HAL_SPI_DISABLE_IT (hspi, SPI_IT_RXNE | SPI_IT_TXE | SPI_IT_ERR);
                        spi->callback->onSpiError (errorCode);
                }
                return;
        }
}
