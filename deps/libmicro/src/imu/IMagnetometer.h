/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_IMAGNETOMETER_H
#define LIB_MICRO_IMAGNETOMETER_H

#include <cstdint>

struct IMagnetometer {

        struct MData {
                int16_t x;
                int16_t y;
                int16_t z;
        };

        virtual ~IMagnetometer () {}
        virtual MData getMData () const = 0;
};

#endif // IACCELEROMETER_H
