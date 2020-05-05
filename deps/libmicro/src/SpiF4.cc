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
//extern "C" void SPI3_IRQHandler () { spiIRQHandler (Spi::spi3); }

/*****************************************************************************/

void spiIRQHandler (Spi *spi)
{
        // TODO
}
