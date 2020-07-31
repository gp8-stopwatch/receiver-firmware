/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanProtocol.h"
#include "CanFrame.h"
#include "Debug.h"
#include <gsl/gsl>

/*****************************************************************************/

void CanProtocol::onCanNewFrame (CanFrame const &frame)
{
        // TODO set filters and masks to avoid this situation
        if (frame.id == uid) {
                return;
        }

        if (frame.dlc >= 1 && callback != nullptr) {
                auto messageId = frame.data[0];

#ifdef WITH_CAN_TRIGGER
                if (Messages (messageId) == Messages::TRIGGER) {
                        uint8_t ii[4] = {frame.data[1], frame.data[2], frame.data[3], frame.data[4]};
                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii);
                        callback->onTrigger ();
                }
                else if (Messages (messageId) == Messages::NO_IR) {
                        callback->onNoIr ();
                }
#endif
                if (Messages (messageId) == Messages::INFO_REQ) {
                        Expects (beam);
                        BeamState state;

                        if (!beam->isActive ()) {
                                state = BeamState::blind;
                        }
                        else {
                                state = (beam->isBeamPresent ()) ? (BeamState::yes) : (BeamState::no);
                        }

                        can.send (CanFrame{uid, true, 3, uint8_t (Messages::INFO_RESP), uint8_t (deviceType), uint8_t (state)}, 0);
                }
                else if (Messages (messageId) == Messages::INFO_RESP) {
                        if (!lastInfoResponseData.full ()) {
                                lastInfoResponseData.emplace_back (frame.id, DeviceType (frame.data[1]), BeamState (frame.data[2]));
                        }
                }
        }
}

/*****************************************************************************/

void CanProtocol::onCanError (uint32_t e) {}

/*****************************************************************************/

void CanProtocol::sendTrigger (uint32_t time)
{
        auto *p = reinterpret_cast<uint8_t *> (&time);
        can.send (CanFrame{uid, true, 5, uint8_t (Messages::TRIGGER), *p, *(p + 1), *(p + 2), *(p + 3)}, 0);
}
