/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanProtocol.h"
#include "CanFrame.h"
#include "Config.h"
#include "Container.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "StopWatch.h"
#include <gsl/gsl>

/****************************************************************************/

std::optional<InfoRespData> frameToInfoResp (CanFrame const &frame)
{
        if (frame.dlc < 4) {
                return {};
        }

        InfoRespData resp;
        resp.uid = frame.id;
        resp.deviceType = DeviceType (frame.data.at (1));
        uint8_t b = frame.data.at (2);
        resp.active = bool (b & 0x01);
        resp.beamState = BeamState ((b & 0x02) >> 1);
        resp.noiseState = NoiseState ((b & 0x04) >> 2);
        resp.noiseLevel = frame.data.at (3);
        return resp;
}

/****************************************************************************/

auto infoRespToFrame (InfoRespData const &rsp)
{
        CanFrame frame;
        frame.id = rsp.uid;
        frame.extended = true;
        frame.data.at (0) = uint8_t (Message::INFO_RESP);
        frame.data.at (1) = uint8_t (rsp.deviceType);
        frame.data.at (2) = uint8_t (rsp.active) | uint8_t (rsp.beamState) << 1 | uint8_t (rsp.noiseState) << 2;
        frame.data.at (3) = rsp.noiseLevel;
        frame.dlc = 4;
        return frame;
}

/*****************************************************************************/

void CanProtocol::onCanNewFrame (CanFrame const &frame)
{

        if (frame.id == uid || frame.dlc == 0) {
                return;
        }

        auto messageId = Message (frame.data[0]);

        switch (messageId) {
#ifdef IS_CAN_MASTER
        case Message::INFO_RESP:
                if (!lastInfoResponseData.full ()) {
                        auto irsp = frameToInfoResp (frame);

                        if (!irsp) {
                                // Log?
                                return;
                        }

                        lastInfoResponseData.push_back (*irsp);
                }
                break;

        case Message::CONFIG_REQUEST:
                if (configResponseTimer.isExpired ()) {
                        sendConfigResp ();
                        configResponseTimer.start (1000);
                }
                break;

        case Message::NO_BEAM:
        case Message::NOISE:
                if (callback != nullptr) {
                        callback->onMessage (Message (messageId));
                }

                break;
#endif

        case Message::INFO_REQ: {
                InfoRespData myData = getMyOwnInfo ();

                if (!can.send (infoRespToFrame (myData))) {
                        // Error_Handler (); // TODO remove
                }
        } break;

        case Message::CONFIG_RESP:
                if (frame.dlc < 3) {
                        return;
                }

                getConfig ().setMinTriggerEventMs (uint16_t (frame.data[1]) | (uint16_t (frame.data[2]) << 8));
                cfg::changed () = true;
                break;

        default:
                break;
        }
}

/*****************************************************************************/

void CanProtocol::onCanError (uint32_t e) {}

/*****************************************************************************/

void CanProtocol::sendInfoRequest ()
{
        lastInfoResponseData.clear ();
        can.send (CanFrame{uid, true, 1, uint8_t (Message::INFO_REQ)}, CAN_SEND_TIMEOUT);
}

/****************************************************************************/

void CanProtocol::sendConfigResp ()
{
        uint16_t tmp = getConfig ().getMinTreggerEventMs ();
        can.send (CanFrame{uid, true, 3, uint8_t (Message::CONFIG_RESP), uint8_t (tmp & 0xff), uint8_t (tmp >> 8)}, CAN_SEND_TIMEOUT);
}
