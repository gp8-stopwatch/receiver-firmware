/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Hal.h"
#include <cstdint>
#include <etl/vector.h>

struct CanFrame {
        const static uint8_t DEFAULT_BYTE = 0x55;
        using Data = etl::vector<uint8_t, 8>;

        CanFrame ();
        CanFrame (uint32_t id, bool extended, uint8_t dlc, uint8_t data0, uint8_t data1 = DEFAULT_BYTE, uint8_t data2 = DEFAULT_BYTE,
                  uint8_t data3 = DEFAULT_BYTE, uint8_t data4 = DEFAULT_BYTE, uint8_t data5 = DEFAULT_BYTE, uint8_t data6 = DEFAULT_BYTE,
                  uint8_t data7 = DEFAULT_BYTE);

#ifndef UNIT_TEST
        CanFrame (CAN_RxHeaderTypeDef const &header, uint8_t const *data);
        CAN_TxHeaderTypeDef toNative () const;
#endif

        bool isExtended () const { return extended; }
        uint32_t getId () const { return id; }
        Data &getData () { return data; }
        Data const &getData () const { return data; }
        size_t getDlc () const { return data.size (); }

private:
        uint32_t id;
        bool extended;
        Data data;
};
