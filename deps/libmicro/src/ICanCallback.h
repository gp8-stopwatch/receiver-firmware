/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICANCALLBACK_H
#define ICANCALLBACK_H

#include "CanFrame.h"

/**
 * Callback class. Observer pattern.
 */
struct ICanCallback {
        enum Error { NO_ERROR, SEND_TIMEOUT = 0x80000000 };
        virtual ~ICanCallback () {}
        virtual void onCanNewFrame (CanFrame const &) = 0;
        virtual void onCanError (uint32_t e) = 0;
};

#endif // ICANCALLBACK_H
