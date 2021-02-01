/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Can.h"
#include "ICanCallback.h"
#include "InfraRedBeamExti.h"
#include "Types.h"
#include <cstdint>
#include <etl/vector.h>
#include <functional>
#include <utility>

/*****************************************************************************/

enum class Message : uint8_t { /* START, STOP, */ LOOP, NO_IR, INFO_REQ, INFO_RESP, NOISE };

/*****************************************************************************/

struct IProtocolCallback {
        IProtocolCallback () = default;
        IProtocolCallback (IProtocolCallback const &) = default;
        IProtocolCallback &operator= (IProtocolCallback const &) = default;
        IProtocolCallback (IProtocolCallback &&) = default;
        IProtocolCallback &operator= (IProtocolCallback &&) = default;
        virtual ~IProtocolCallback () = default;

        virtual void onMessage (Message msg, Result time) = 0;
};

/*****************************************************************************/

enum class BeamState : uint8_t { yes, no, blind };

struct InfoRespData {
        InfoRespData (uint32_t u, DeviceType dt, BeamState bs) : uid{u}, deviceType{dt}, beamState{bs} {}

        uint32_t uid;
        DeviceType deviceType;
        BeamState beamState;
};

using InfoRespDataCollection = etl::vector<InfoRespData, 16>;

/*****************************************************************************/

class CanProtocol : public ICanCallback {
public:
        CanProtocol (Can &can, uint32_t u, DeviceType dt) : can (can), uid (u & 0x1FFFFFFF), deviceType{dt} {}

        void sendTrigger (Message msg, Result time);
        void sendNoIr () { can.send (CanFrame{uid, true, 1, uint8_t (Message::NO_IR)}, CAN_SEND_TIMEOUT); }

        void sendInfoRequest ();
        void setCallback (IProtocolCallback *cb) { callback = cb; }

        InfoRespDataCollection &getInfoRespDataCollection () { return lastInfoResponseData; }
        InfoRespDataCollection const &getInfoRespDataCollection () const { return lastInfoResponseData; }

        void setBeam (IInfraRedBeam *b) { beam = b; }

private:
        void onCanNewFrame (CanFrame const &frame) override;
        void onCanError (uint32_t e) override;

        /*--------------------------------------------------------------------------*/

        Can &can;
        IProtocolCallback *callback{};
        uint32_t uid;
        DeviceType deviceType;
        IInfraRedBeam *beam{};
        InfoRespDataCollection lastInfoResponseData;
};
