/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_IIMU_H
#define LIB_MICRO_IIMU_H

#include "IAccelerometer.h"
#include "IGyroscope.h"
#include "IMagnetometer.h"
#include <cstdint>

struct IImu : public IGyroscope, public IAccelerometer, public IMagnetometer {
        virtual ~IImu () {}
//        virtual Data getData () const = 0;
};

#endif // IACCELEROMETER_H
