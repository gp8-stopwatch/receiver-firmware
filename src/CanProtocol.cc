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
#include "ErrorHandler.h"
#include <gsl/gsl>

/*****************************************************************************/

void CanProtocol::onCanNewFrame (CanFrame const &frame)
{
        // TODO set filters and masks to avoid this situation
        if (frame.getId () == uid) {
                return;
        }

        if (frame.getDlc () >= 1 && callback != nullptr) {
                auto messageId = frame.getData ().at (0);

                if (Messages (messageId) == Messages::TRIGGER) {
                        std::array<uint8_t, 4> ii{frame.getData ().at (1), frame.getData ().at (2), frame.getData ().at (3),
                                                  frame.getData ().at (4)};

                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii.data ());
                        callback->onTrigger ();
                }
#ifdef WITH_CHECK_SENSOR_STATUS
                else if (Messages (messageId) == Messages::NO_IR) {
                        callback->onNoIr ();
                }
#endif
                else if (Messages (messageId) == Messages::INFO_REQ) {
                        Expects (beam);
                        BeamState state;

                        if (!beam->isActive ()) {
                                state = BeamState::blind;
                        }
                        else {
                                state = (beam->isBeamPresent ()) ? (BeamState::yes) : (BeamState::no);
                        }

                        if (!can.send (CanFrame{uid, true, 3, uint8_t (Messages::INFO_RESP), uint8_t (deviceType), uint8_t (state)})) {
                                // Error_Handler (); // TODO remove
                        }
                }
                else if (Messages (messageId) == Messages::INFO_RESP) {
                        if (!lastInfoResponseData.full ()) {
                                lastInfoResponseData.emplace_back (frame.getId (), DeviceType (frame.getData ().at (1)),
                                                                   BeamState (frame.getData ().at (2)));
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
        can.send (CanFrame{uid, true, 5, uint8_t (Messages::TRIGGER), *p, *(p + 1), *(p + 2), *(p + 3)});
}
