/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanFrame.h"
#include <cstring>

CanFrame::CanFrame () : id (0), extended (false), dlc (0) { memset (data, 0, sizeof (data)); }

CanFrame::CanFrame (uint32_t id, bool extended, uint8_t dlc, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4,
                    uint8_t data5, uint8_t data6, uint8_t data7)
    : id (id), extended (extended), dlc (dlc)
{
        data[0] = data0;
        data[1] = data1;
        data[2] = data2;
        data[3] = data3;
        data[4] = data4;
        data[5] = data5;
        data[6] = data6;
        data[7] = data7;
}

/*****************************************************************************/
#ifndef UNIT_TEST

CanFrame::CanFrame (CanRxMsgTypeDef const &frame)
{
        if (frame.IDE == CAN_ID_EXT) {
                id = frame.ExtId;
                extended = true;
        }
        else {
                id = frame.StdId;
                extended = true;
        }

        dlc = frame.DLC;
        memset (data, 0, 8);
        memcpy (data, frame.Data, frame.DLC);
}

/*****************************************************************************/

CanTxMsgTypeDef CanFrame::toNative () const
{
        CanTxMsgTypeDef native;

        if (extended) {
                native.StdId = 0x00;
                native.ExtId = id;
                native.IDE = CAN_ID_EXT;
        }
        else {
                native.StdId = id;
                native.ExtId = 0x00;
                native.IDE = CAN_ID_STD;
        }

        native.RTR = CAN_RTR_DATA;
        native.DLC = dlc;
        memset (native.Data, 0, 8);
        memcpy (native.Data, data, dlc);
        return native;
}

#endif
