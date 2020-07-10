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
                if (Messages (frame.data[0]) == Messages::START && onStart) {
                        // uint8_t ii[4] = {frame.data[1], frame.data[2], frame.data[3], frame.data[4]};
                        remoteStopTime = 0;
                        onStart ();
                }

                if (Messages (frame.data[0]) == Messages::LOOP_START && onLoopStart) {
                        uint8_t ii[4] = {frame.data[1], frame.data[2], frame.data[3], frame.data[4]};
                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii);
                        onLoopStart ();
                }

#ifdef ACCEPT_CAN_BUS_STOP
                if (Messages (frame.data[0]) == Messages::STOP && onStop) {
                        // remoteStop = true;
                        uint8_t ii[4] = {frame.data[1], frame.data[2], frame.data[3], frame.data[4]};
                        remoteStopTime = *reinterpret_cast<uint32_t *> (ii);
                        onStop ();
                }
#endif
        }
}

/*****************************************************************************/

void CanProtocol::onCanError (uint32_t e) {}

/*****************************************************************************/

void CanProtocol::sendStart () { can.send (CanFrame{uid, true, 5, uint8_t (Messages::START), 0, 0, 0, 0}, 0); }

/*****************************************************************************/

void CanProtocol::sendLoopStart (uint32_t time)
{
        auto *p = reinterpret_cast<uint8_t *> (&time);
        can.send (CanFrame{uid, true, 5, uint8_t (Messages::LOOP_START), *p, *(p + 1), *(p + 2), *(p + 3)}, 0);
}

/*****************************************************************************/

void CanProtocol::sendStop (uint32_t time)
{
        auto *p = reinterpret_cast<uint8_t *> (&time);
        can.send (CanFrame{uid, true, 5, uint8_t (Messages::STOP), *p, *(p + 1), *(p + 2), *(p + 3)}, 0);
}
