/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanFrame.h"
#include <algorithm>
#include <cstring>

CanFrame::CanFrame () : id (0), extended (false) {}

CanFrame::CanFrame (uint32_t id, bool extended, uint8_t dlc, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4,
                    uint8_t data5, uint8_t data6, uint8_t data7)
    : id (id), extended (extended)
{
        data.resize (8);
        data[0] = data0;
        data[1] = data1;
        data[2] = data2;
        data[3] = data3;
        data[4] = data4;
        data[5] = data5;
        data[6] = data6;
        data[7] = data7;
        data.resize (dlc);
}

/*****************************************************************************/
#ifndef UNIT_TEST

CanFrame::CanFrame (CAN_RxHeaderTypeDef const &header, uint8_t const *data)
{
        if (header.IDE == CAN_ID_EXT) {
                id = header.ExtId;
                extended = true;
        }
        else {
                id = header.StdId;
                extended = true;
        }

        this->data.resize (header.DLC);
        std::copy_n (data, header.DLC, this->data.begin ());
}

/*****************************************************************************/

CAN_TxHeaderTypeDef CanFrame::toNative () const
{
        CAN_TxHeaderTypeDef native{};

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
        native.DLC = data.size ();
        return native;
}

#endif
