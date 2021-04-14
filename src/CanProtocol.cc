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
#include "Debug.h"
#include "ErrorHandler.h"
#include "StopWatch.h"
#include <gsl/gsl>

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
                        lastInfoResponseData.emplace_back (frame.id, DeviceType (frame.data[1]), BeamState (frame.data[2]));
                }
                break;

        case Message::CONFIG_REQUEST:
                if (configResponseTimer.isExpired ()) {
                        sendConfigRequest ();
                        configResponseTimer.start (1000);
                }
                break;
#endif

        case Message::INFO_REQ: {
                if (frame.dlc < 3) {
                        return;
                }

                BeamState state{};

                if (!ir->isActive ()) {
                        state = BeamState::blind;
                }
                else {
                        // TODO detect noise
                        // state = (ir->getBeamState () == IrBeam::triggerFalling) ? (BeamState::yes) : (BeamState::no);
                }

                if (!can.send (CanFrame{uid, true, 3, uint8_t (Message::INFO_RESP), uint8_t (deviceType), uint8_t (state)}, CAN_SEND_TIMEOUT)) {
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

        case Message::NO_BEAM:
        case Message::NOISE:
                if (callback != nullptr) {
                        callback->onMessage (Message (messageId));
                }

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
