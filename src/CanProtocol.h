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
#include "InfraRedBeamModulated.h"
#include "Types.h"
#include <cstdint>
#include <etl/vector.h>
#include <functional>
#include <utility>

/*****************************************************************************/

struct IProtocolCallback {
        IProtocolCallback () = default;
        IProtocolCallback (IProtocolCallback const &) = default;
        IProtocolCallback &operator= (IProtocolCallback const &) = default;
        IProtocolCallback (IProtocolCallback &&) = default;
        IProtocolCallback &operator= (IProtocolCallback &&) = default;
        virtual ~IProtocolCallback () = default;

        virtual void onTrigger () = 0;
        virtual void onNoIr () = 0;
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
        enum class Messages : uint8_t { TRIGGER, NO_IR, INFO_REQ, INFO_RESP };

        CanProtocol (Can &can, uint32_t u, DeviceType dt) : can (can), uid (u & 0x1FFFFFFF), deviceType{dt} {}

        void sendTrigger (uint32_t time);
        void sendNoIr () { can.send (CanFrame{uid, true, 1, uint8_t (Messages::NO_IR)}, 0); }

        void sendInfoRequest ()
        {
                lastInfoResponseData.clear ();
                can.send (CanFrame{uid, true, 1, uint8_t (Messages::INFO_REQ)}, 0);
        }

        void setCallback (IProtocolCallback *cb) { callback = cb; }

        /// This is a little hack to get rid of passing the time in the event, which would be difficult, since an event is an enum.
        uint32_t getLastRemoteStopTime () const { return remoteStopTime; }
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
        mutable uint32_t remoteStopTime{};
        DeviceType deviceType;
        IInfraRedBeam *beam{};
        InfoRespDataCollection lastInfoResponseData;
};
