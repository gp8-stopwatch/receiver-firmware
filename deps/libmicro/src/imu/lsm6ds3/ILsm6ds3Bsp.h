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
struct ILsm6ds3Bsp {
        virtual ~ILsm6ds3Bsp () {}

//        virtual void itConfig () = 0;
        virtual void writeRegister (uint8_t addr, uint8_t val) = 0;
        virtual uint8_t readRegister (uint8_t addr) = 0;
        virtual void writeRegister (uint8_t addr, uint8_t *buffer, uint16_t len) = 0;
        virtual void readRegister (uint8_t addr, uint8_t *buffer, uint16_t len) = 0;
};

#endif // ILIS3DSHBSP_H
