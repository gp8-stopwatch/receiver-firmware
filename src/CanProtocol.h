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
#include <functional>
#include <utility>

/*****************************************************************************/

class CanProtocol : public ICanCallback {
public:
        using Callback = std::function<void (void)>;
        enum class Messages : uint8_t { START, STOP };

        CanProtocol (Can &can, uint32_t u) : can (can), uid (u & 0x1FFFFFFF) {}

        void sendStart ();
        void sendStop (uint32_t time);

        void onCanNewFrame (CanFrame const &frame) override;
        void onCanError (uint32_t e) override;

        //         bool isRemoteStartAndClear () const
        //         {
        //                 if (remoteStart) {
        //                         remoteStart = false;
        //                         return true;
        //                 }

        //                 return false;
        //         }

        //         std::pair<bool, uint32_t> isRemoteStopAndClear () const
        //         {
        // #ifdef ACCEPT_CAN_BUS_STOP
        //                 if (remoteStop) {
        //                         std::pair<bool, uint32_t> p{true, remoteStopTime};
        //                         remoteStop = false;
        //                         return p;
        //                 }
        // #endif

        //                 return {false, 0};
        //         }

        // void run ();

        void setOnStart (Callback const &callback) { onStart = callback; }
        void setOnStop (Callback const &callback) { onStop = callback; }

        /// This is a little hack to get rid of passing the time in the event, which would be difficult, since an event is an enum.
        uint32_t getLastRemoteStopTime () const { return remoteStopTime; }

private:
        Can &can;
        uint32_t uid;

        Callback onStart;
        Callback onStop;

        mutable bool remoteStart = false;
        mutable bool remoteStop = false;
        bool sendStartRq = false;
        bool sendStopRq = false;
        mutable uint32_t remoteStopTime = 0;
        uint32_t sendStopTimeRq = 0;
};
