/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BLINKER_H
#define BLINKER_H

#include "ErrorHandler.h"
#include "Gpio.h"
#include "Timer.h"
#include <cstdarg>
#include <cstdint>
#include <cstdlib>

/**
 * Blinking pattern. Initially it is running.
 */
template <size_t SLOTS_NO> class Blinker {
public:
        /**
         * Variadic constructor taking SLOTS_NO integer arguments.
         * First argument is the time (in ms) during the Gpio is turned off.
         * Second argument is the time dusring the Gpio is turned on.
         * And so on.
         */
        Blinker (Gpio &g, int i, ...);
        Blinker (Blinker const &) = delete;
        Blinker (Blinker &&) = delete;
        Blinker &operator= (Blinker const &) = delete;
        Blinker &operator= (Blinker &&) = delete;
        ~Blinker () = default;

        /// Starts
        void start ();
        void stop () { running = false; }
        void run ();

        void setTimeSlot (size_t slotNo, uint16_t period);

public:
        bool running = true;
        uint8_t curentSlotNo = 0;
        uint16_t timeSlot[SLOTS_NO] = { 0 };
        Timer timer;
        Gpio &gpio;
};

/*****************************************************************************/

template <size_t SLOTS_NO> Blinker<SLOTS_NO>::Blinker (Gpio &g, int slots, ...) : gpio (g)
{
        va_list args;
        va_start (args, slots);
        timeSlot[0] = slots;

        for (int i = 1; i < SLOTS_NO; ++i) {
                timeSlot[i] = va_arg (args, int);
        }

        va_end (args);
}

/*****************************************************************************/

template <size_t SLOTS_NO> void Blinker<SLOTS_NO>::start ()
{
        running = true;
        timer.start (timeSlot[curentSlotNo++]);
        curentSlotNo %= SLOTS_NO;
}

/*****************************************************************************/

template <size_t SLOTS_NO> void Blinker<SLOTS_NO>::run ()
{
        if (running && timer.isExpired ()) {
                // Even and 0 means led turned off.
                gpio.set (curentSlotNo % 2 != 0);
                start ();
        }
}

template <size_t SLOTS_NO> void Blinker<SLOTS_NO>::setTimeSlot (size_t slotNo, uint16_t period)
{

        if (slotNo >= SLOTS_NO) {
                Error_Handler ();
        }

        timeSlot[slotNo] = period;
}

#endif // BLINKER_H
