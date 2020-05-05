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
#include <cstdint>
#include <cstdlib>

class I2c {
public:
        I2c (I2C_Type *instance, uint32_t clockSpeed = 10000);

        enum { DEFAULT_TIMEOUT = -1 };
        bool read (uint8_t devAddr, uint8_t *data, size_t length, bool stopCondition = true, int timeout = DEFAULT_TIMEOUT);
        bool write (uint8_t devAddr, uint8_t *data, size_t length, bool stopCondition = true, int timeout = DEFAULT_TIMEOUT);

//        void read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, int timeout = DEFAULT_TIMEOUT);
//        void write (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, int timeout = DEFAULT_TIMEOUT);

private:
        I2C_Type *i2c;
};

#endif // I2C_H
