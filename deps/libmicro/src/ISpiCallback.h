/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ISPICALLBACK_H
#define ISPICALLBACK_H

#include <cstdint>
#include <cstdlib>

struct ISpiCallback {
        virtual ~ISpiCallback () = default;
        virtual void onTxComplete () = 0;
        virtual void onRxComplete (const uint8_t *data, size_t size) = 0;
        virtual void onSpiError (uint32_t) = 0;
};

#endif // ISPICALLBACK_H
