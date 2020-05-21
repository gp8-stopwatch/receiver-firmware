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

/*****************************************************************************/

void CanProtocol::onCanNewFrame (CanFrame const &frame)
{
        // TODO set filters and masks to avoid this situation
        if (frame.id == uid) {
                return;
        }

        if (frame.dlc >= 1) {
                if (Messages (frame.data[0]) == Messages::START) {
                        remoteStart = true;
                }

                if (Messages (frame.data[0]) == Messages::STOP) {
                        remoteStop = true;
                        uint8_t ii[4] = { frame.data[1], frame.data[2], frame.data[3], frame.data[4] };
                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii);
                }
        }
}

/*****************************************************************************/

void CanProtocol::onCanError (uint32_t e) {}

/*****************************************************************************/

void CanProtocol::sendStart () { sendStartRq = true; }

/*****************************************************************************/

void CanProtocol::sendStop (uint32_t time)
{
        sendStopRq = true;
        sendStopTimeRq = time;
}

/*****************************************************************************/

void CanProtocol::run ()
{
        if (sendStopRq) {
                uint8_t *p = reinterpret_cast<uint8_t *> (&sendStopTimeRq);
                // debug->println ("C");
                can.send (CanFrame{ uid, true, 5, uint8_t (Messages::STOP), *p, *(p + 1), *(p + 2), *(p + 3) }, 0);
                // debug->println (".");
                sendStopRq = false;
        }
        if (sendStartRq) {
                // debug->println ("D");
                can.send (CanFrame{ uid, true, 1, uint8_t (Messages::START) }, 0);
                // debug->println (".");
                sendStartRq = false;
        }
}