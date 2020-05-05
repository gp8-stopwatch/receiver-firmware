/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LSM6DS3SPIBSP_H
#define LSM6DS3SPIBSP_H

#include "ILsm6ds3Bsp.h"
#include "Spi.h"

/**
 * Interfejs dla klas chcących obsłurzyć przerwania akcleerometru. Te funkcje wykonają się w ISR.
 */
// struct ILsm6ds3SpiBspListener {
//        virtual ~ILsm6ds3SpiBspListener () {}
//        virtual void onInterrupt1 () = 0;
//        virtual void onInterrupt2 () = 0;
//};

/**
 * Low level implementation. Reimplement only this one class when moving to another
 * board / µC.
 */
class Lsm6ds3SpiBsp : public ILsm6ds3Bsp {
public:
        Lsm6ds3SpiBsp (Spi *s) : spi (s) /*lis3dshBspListener (nullptr) */ {}
        virtual ~Lsm6ds3SpiBsp () {}

        //        void itConfig ();
        void writeRegister (uint8_t addr, uint8_t val);
        uint8_t readRegister(uint8_t addr);
        void writeRegister (uint8_t addr, uint8_t *buffer, uint16_t len);
        void readRegister (uint8_t addr, uint8_t *buffer, uint16_t len);

        //        void setLsm6ds3BspListener (ILsm6ds3SpiBspListener *l) { lis3dshBspListener = l; }

private:
        enum { WRITE_MASK = 0x7f, READ_MASK = 0x80 };

        //        ILsm6ds3SpiBspListener *lis3dshBspListener;
        Spi *spi;
};

#endif // LIS3DSHSPIBSP_H
