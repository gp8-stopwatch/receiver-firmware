/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef IACCELEROMETER_H
#define IACCELEROMETER_H

#include <cstdint>

struct IAccelerometer {

        struct AData {
                int16_t x;
                int16_t y;
                int16_t z;
        };

        virtual ~IAccelerometer () = default;
        virtual AData getAData () const = 0;
};

#endif // IACCELEROMETER_H
