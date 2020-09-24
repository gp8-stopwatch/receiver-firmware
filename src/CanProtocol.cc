/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanProtocol.h"
#include "CanFrame.h"
#include "Container.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "StopWatch.h"
#include <gsl/gsl>

/*****************************************************************************/

void CanProtocol::onCanNewFrame (CanFrame const &frame)
{
        // TODO set filters and masks to avoid this situation
        if (frame.id == uid) {
                return;
        }

        Result remoteStopTime{};

        if (frame.dlc >= 1 && callback != nullptr) {
                auto messageId = frame.data[0];

                if (Message (messageId) == Message::INFO_REQ) {
                        Expects (beam);
                        uint16_t tmp = uint16_t (frame.data[1]) | uint16_t (frame.data[2]) << 8;
                        getConfig ().setBlindTime (tmp);
                        BeamState state;

                        if (!beam->isActive ()) {
                                state = BeamState::blind;
                        }
                        else {
                                // TODO detect noise
                                state = (beam->getBeamState () == IrBeam::present) ? (BeamState::yes) : (BeamState::no);
                        }

                        if (!can.send (CanFrame{uid, true, 3, uint8_t (Message::INFO_RESP), uint8_t (deviceType), uint8_t (state)},
                                       CAN_SEND_TIMEOUT)) {
                                // Error_Handler (); // TODO remove
                        }
                }
#ifdef WITH_CHECK_SENSOR_STATUS
                else if (Message (messageId) == Message::INFO_RESP) {
                        if (!lastInfoResponseData.full ()) {
                                lastInfoResponseData.emplace_back (frame.id, DeviceType (frame.data[1]), BeamState (frame.data[2]));
                        }
                }
#endif
                else {
                        uint8_t ii[4] = {frame.data[1], frame.data[2], frame.data[3], frame.data[4]};
                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii);
                        callback->onMessage (Message (messageId), remoteStopTime);
                }
        }
}

/*****************************************************************************/

void CanProtocol::onCanError (uint32_t e) {}

/*****************************************************************************/

void CanProtocol::sendTrigger (Message msg, Result time)
{
        auto *p = reinterpret_cast<uint8_t *> (&time);
        can.send (CanFrame{uid, true, 5, uint8_t (msg), *p, *(p + 1), *(p + 2), *(p + 3)}, 0);
}

/*****************************************************************************/

void CanProtocol::sendInfoRequest ()
{
        lastInfoResponseData.clear ();
        uint16_t tmp = getConfig ().getBlindTime ();
        can.send (CanFrame{uid, true, 3, uint8_t (Message::INFO_REQ), uint8_t (tmp & 0xff), uint8_t (tmp >> 8)}, CAN_SEND_TIMEOUT);
}
