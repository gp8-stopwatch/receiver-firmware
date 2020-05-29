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
#include <cstdint>
#include <utility>

/*****************************************************************************/

class CanProtocol : public ICanCallback {
public:
        CanProtocol (Can &can, uint32_t u) : can (can), uid (u & 0x1FFFFFFF) {}
        virtual ~CanProtocol () override = default;

        void sendStart ();
        void sendStop (uint32_t time);

        void onCanNewFrame (CanFrame const &) override;
        void onCanError (uint32_t e) override;

        bool isRemoteStartAndClear () const
        {
                if (remoteStart) {
                        remoteStart = false;
                        return true;
                }

                return false;
        }

        std::pair<bool, uint32_t> isRemoteStopAndClear () const
        {
#ifdef ACCEPT_CAN_BUS_STOP
                if (remoteStop) {
                        std::pair<bool, uint32_t> p{true, remoteStopTime};
                        remoteStop = false;
                        return p;
                }
#endif

                return {false, 0};
        }

        enum class Messages : uint8_t { START, STOP };

        void run ();

private:
        Can &can;
        uint32_t uid;

        mutable bool remoteStart = false;
        mutable bool remoteStop = false;
        bool sendStartRq = false;
        bool sendStopRq = false;
        mutable uint32_t remoteStopTime = 0;
        uint32_t sendStopTimeRq = 0;
};
