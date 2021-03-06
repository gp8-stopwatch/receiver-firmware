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
#include "Timer.h"
#include "Types.h"
#include "detector/IrTriggerDetector.h"
#include <cstdint>
#include <etl/vector.h>
#include <functional>
#include <utility>

/*****************************************************************************/

enum class Message : uint8_t {
        NO_BEAM,        // slave -> master
        NOISE,          // slave -> master
        INFO_REQ,       // master -> slave, and then:
        INFO_RESP,      // slave -> master
        CONFIG_REQUEST, // slave -> master, and then:
        CONFIG_RESP,    // master -> slave
        SYNCHRONIZATION // slave -> master
};

/*****************************************************************************/

struct IProtocolCallback {
        IProtocolCallback () = default;
        IProtocolCallback (IProtocolCallback const &) = default;
        IProtocolCallback &operator= (IProtocolCallback const &) = default;
        IProtocolCallback (IProtocolCallback &&) = default;
        IProtocolCallback &operator= (IProtocolCallback &&) = default;
        virtual ~IProtocolCallback () = default;

        virtual void onMessage (Message msg, Result1usLS time) = 0;
};

using InfoRespDataCollection = etl::vector<InfoRespData, 16>;

/*****************************************************************************/

class CanProtocol : public ICanCallback {
public:
        CanProtocol (Can &can, uint32_t u, DeviceType dt) : can (can), uid (u & 0x1FFFFFFF), deviceType{dt} {}

        /*--------------------------------------------------------------------------*/
        /* Requestes                                                                */
        /*--------------------------------------------------------------------------*/

        void sendNoBeam () { can.send (CanFrame{uid, true, 1, uint8_t (Message::NO_BEAM)}, CAN_SEND_TIMEOUT); }
        void sendNoise () { can.send (CanFrame{uid, true, 1, uint8_t (Message::NOISE)}, CAN_SEND_TIMEOUT); }
        void sendInfoRequest ();
        void sendConfigRequest () { can.send (CanFrame{uid, true, 1, uint8_t (Message::CONFIG_REQUEST)}, CAN_SEND_TIMEOUT); }
        void sendConfigResp ();
        void sendSynchronization (Result1usLS delay);

        /*--------------------------------------------------------------------------*/
        /* Responses asynchronously                                                 */
        /*--------------------------------------------------------------------------*/

        void setCallback (IProtocolCallback *cb) { callback = cb; }

        /*--------------------------------------------------------------------------*/
        /* Responses synchronously                                                  */
        /*--------------------------------------------------------------------------*/

        InfoRespDataCollection &getInfoRespDataCollection () { return lastInfoResponseData; }
        InfoRespDataCollection const &getInfoRespDataCollection () const { return lastInfoResponseData; }

        void setIrTriggerDetector (IrTriggerDetector *b) { ir = b; }

private:
        void onCanNewFrame (CanFrame const &frame) override;
        void onCanError (uint32_t e) override;

        /*--------------------------------------------------------------------------*/

        Can &can;
        IProtocolCallback *callback{};
        uint32_t uid;
        DeviceType deviceType;
        IrTriggerDetector *ir{};
        InfoRespDataCollection lastInfoResponseData;
        Timer configResponseTimer{};
};
