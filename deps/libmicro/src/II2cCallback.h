/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef I2CCALLBACK_H
#define I2CCALLBACK_H

#include <cstdint>
#include <cstdlib>

struct II2cCallback {
        virtual ~II2cCallback () = default;
        virtual void onTxComplete (uint16_t address, const uint8_t *data, size_t size) = 0;
        virtual void onRxComplete (uint16_t address, const uint8_t *data, size_t size) = 0;
        virtual void onI2cError (uint32_t) = 0;
};

#endif // I2CCALLBACK_H
