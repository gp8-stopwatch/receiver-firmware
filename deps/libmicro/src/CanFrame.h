/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef __CAN_FRAME_H__
#define __CAN_FRAME_H__

#include "Hal.h"
#include <cstdint>

struct CanFrame {
        const static uint8_t DEFAULT_BYTE = 0x55;

        CanFrame ();
        CanFrame (uint32_t id, bool extended, uint8_t dlc, uint8_t data0, uint8_t data1 = DEFAULT_BYTE, uint8_t data2 = DEFAULT_BYTE,
                  uint8_t data3 = DEFAULT_BYTE, uint8_t data4 = DEFAULT_BYTE, uint8_t data5 = DEFAULT_BYTE, uint8_t data6 = DEFAULT_BYTE,
                  uint8_t data7 = DEFAULT_BYTE);

#ifndef UNIT_TEST
        CanFrame (CanRxMsgTypeDef const &frame);
        CanTxMsgTypeDef toNative () const;
#endif

        operator bool () const { return id != 0; }

        uint32_t id;
        bool extended;
        uint8_t dlc;
        uint8_t data[8];
};

#endif
