/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SYMAX5HWRXPROTOCOL_H
#define SYMAX5HWRXPROTOCOL_H

#include "Nrf24L01P.h"

/**
 * Syma X5HW protocol. But unfortunately it seems, that only my particular
 * transmitter will work with this code, because I was unable to confirm how
 * X5HW hops the frequencies. It does it differently from regular X5. Iwas
 * unable to snif for SPI transmission, because it uses Beken bk2461 chip,
 * which is a SoC, which integrates MCS51 µC alongside with nRF compatible
 * transceiverm, so SPI is not exposed. But swiping on all the canals I was
 * able to observe valid ones, and I can develop an algorithm basing only on
 * one transmitter.
 *
 * Code based on:
 * https://github.com/Suxsem/symaxrx
 * https://github.com/DeviationTX/deviation
 */
class SymaX5HWRxProtocol : public Nrf24L01PCallback {
public:
        struct RxValues {
                uint8_t throttle;    // 0...255
                int8_t yaw;          // 127...-127
                int8_t pitch;        // 127...-127
                int8_t roll;         // 127...-127
                int8_t trimYaw;      // 31...-31
                int8_t trimPitch;    // 31...-31
                int8_t trimRoll;     // 31...-31
                int8_t trimThrottle; // 31...-31
                bool video;
                bool picture;
                bool highspeed;
                bool flip;
        };

        SymaX5HWRxProtocol (Nrf24L01P *n);
        virtual ~SymaX5HWRxProtocol () {}

        void onPacket (uint8_t *packet);
        virtual void onRx (uint8_t *data, size_t len);
        virtual void onTx () {}
        virtual void onMaxRt () {}
        void run ();

        std::function<void(RxValues const &)> onRxValues;

private:
        enum State { INIT, BINDING, BINDING2, RECEIVING, STATES_NUM };

        struct StateInfo {
                uint8_t *channels;
                uint8_t *address;
                uint8_t channelsLength;
                uint8_t addressLength;
                uint8_t packetLength;
        };

        StateInfo *stateInfo[STATES_NUM];

        void buildStateInfo ();
        void setStateChannels (State state, uint8_t *channels = nullptr, uint8_t channelsNum = 0);
        void setStateAddress (State state, uint8_t *address = nullptr, uint8_t addressLength = 0);
        void setStatePacketLength (State state);

        void onBindPacket (uint8_t *packet);
        void onBind2Packet (uint8_t *packet);
        void onReceivePacket (uint8_t *packet);

private:
        Nrf24L01P *nrf;
        State state;
        uint8_t *channels;
        uint8_t channelsNum;
        uint8_t currentChannel;
        uint8_t *address;
        uint8_t addressLength;
        uint8_t packetLength;
        uint8_t packetNo;
//        RxValues values[2];
};

#endif // SYMAX5HWRXPROTOCOL_H
