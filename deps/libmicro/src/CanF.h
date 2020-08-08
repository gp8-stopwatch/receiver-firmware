/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "CanFrame.h"
#include "Hal.h"
#include "ICanCallback.h"
#include "stm32f0xx_hal_can.h"
#include <cstdint>

extern "C" void CEC_CAN_IRQHandler ();
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef *);

class Can {
public:
        Can (ICanCallback *callback = nullptr, uint32_t prescaler = 6, uint32_t sjw = CAN_SJW_3TQ, uint32_t bs1 = CAN_BS1_12TQ,
             uint32_t bs2 = CAN_BS2_3TQ);

        /// Synchronous and blocking
        bool send (CanFrame const &buff);

        /// Synchronous and blocking
        // CanFrame read (uint32_t timeoutMs = 1000);
        void setFilterAndMask (uint32_t filter, uint32_t mask, bool extended);

        /// Asyncronous, callback called from ISR.
        void setCanCallback (ICanCallback *c) { callback = c; }

        /// Turns interrupts on or off. TODO Check if synchronous read works when turned on.
        void interrupts (bool on, bool errors = true);

        /// Reset and put in loopback mode.
        void disable ();

        /// Resets (put in sleep mode).
        void reset ();

        /// Modes of operation as described in reference manual chapter 29.4
        enum Mode { SLEEP, INITIALIZATION, NORMAL };

        /// Sets the mode. Blocks.
        void setMode (Mode mode);

        void setBaudratePrescaler (uint32_t prescaler);

        void setAutomaticRetransmission (bool b);

private:
        static void clkEnable (CAN_HandleTypeDef *canX);
        static void clkDisable (CAN_HandleTypeDef *spiX);
        void clkEnable () { clkEnable (&canHandle); }
        void clkDisable () { clkDisable (&canHandle); }

        /*--------------------------------------------------------------------------*/

        friend void CEC_CAN_IRQHandler ();
        friend void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef *);

        CAN_HandleTypeDef canHandle{};
        ICanCallback *callback;
        static Can *can;
};
