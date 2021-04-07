/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Types.h"
#include <cstdint>

struct IDisplay {
        // enum Letters { LETTER_L = 16, LETTER_r, LETTER_H };
        enum Dots { DOT5 = 0x01, DOT4 = 0x02, DOT3 = 0x04, DOT2 = 0x08 };
        enum Icons { BOTTOM_LEFT_ARROW = 0x01, TOP_LEFT_ARROW = 0x02, TOP_RIGHT_AROW = 0x04, MINUS_SIGN = 0x08 };
        enum Batery { FRAME = 0x01, BAR0 = 0x02, BAR1 = 0x04, BAR2 = 0x08, BAR3 = 0x10 };

        virtual ~IDisplay () = default;
        virtual void setDigit (uint8_t number, uint8_t setDigit) = 0;

        virtual uint8_t getDots () const = 0;
        virtual void setDots (uint8_t bitmask) = 0;
        virtual void setDot (uint8_t number, bool on) = 0;

        virtual uint8_t getIcons () const = 0;
        virtual void setIcons (uint8_t bitmask) = 0;

        virtual void setTime (Result10us, Resolution res) = 0;
        virtual void setText (const char *) = 0;

        /**
         * @brief batteryLevel 0 : blinking frame, 1 : frame, 2 : frame & 1 bar, 3 : frame & 2 bars
         * @param level
         */
        virtual void setBatteryLevel (uint8_t level) = 0;
        virtual void setBacklight (bool b) = 0;
        virtual bool getBacklight () const = 0;

        virtual void setBrightness (uint8_t b) = 0;
        virtual uint8_t getBrightness () const = 0;

        // virtual void refresh () = 0;

        virtual void clear () = 0;
        virtual void setFlip (bool b) = 0;

        virtual void setResolution (Resolution res) = 0;

        virtual void setFps (unsigned int fps) = 0;
};

/**
 * For devices without display, to simplify code.
 */
struct FakeDisplay : public IDisplay {
        void setDigit (uint8_t number, uint8_t setDigit) override {}

        uint8_t getDots () const override { return 0; }
        void setDots (uint8_t bitmask) override {}
        void setDot (uint8_t number, bool on) override {}

        uint8_t getIcons () const override { return 0; }
        void setIcons (uint8_t bitmask) override {}

        void setTime (Result10us /* a */, Resolution res) override {}
        void setText (const char * /* a */) override {}

        /**
         * @brief batteryLevel 0 : blinking frame, 1 : frame, 2 : frame & 1 bar, 3 : frame & 2 bars
         * @param level
         */
        void setBatteryLevel (uint8_t level) override {}
        void setBacklight (bool b) override {}
        bool getBacklight () const override { return false; }

        void setBrightness (uint8_t b) override {}
        uint8_t getBrightness () const override { return 0; }

        void refresh () {}

        void clear () override {}
        void setFlip (bool b) override {}

        void setResolution (Resolution res) override {}
        void setFps (unsigned int) override {}
};