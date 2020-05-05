/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ILIS3DSHBSP_H
#define ILIS3DSHBSP_H

#include <cstdint>

/**
 * Abstraction of low level acces to the LIS3DSH. Two implementations are possible :
 * SPI based and I2C based, since LIS supports both of this interfaces.
 */
struct ILis3dshBsp {
        virtual ~ILis3dshBsp () {}

        virtual void init () = 0;
        virtual void itConfig () = 0;
        virtual void write (uint8_t *buffer, uint8_t addr, uint16_t len) = 0;
        virtual void read (uint8_t *buffer, uint8_t addr, uint16_t len) = 0;
};

#endif // ILIS3DSHBSP_H
