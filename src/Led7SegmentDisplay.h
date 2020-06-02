/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Gpio.h"
#include "IDisplay.h"
#include <cstdint>

class Led7SegmentDisplay : public IDisplay {
public:
        template <typename... G>
        Led7SegmentDisplay (Gpio &a, Gpio &b, Gpio &c, Gpio &d, Gpio &e, Gpio &f, Gpio &g, Gpio &dp, G &... dd)
            : segment{&a, &b, &c, &d, &e, &f, &g, &dp}, common{&dd...}
        {
                for (Gpio *g : common) {
                        *g = true;
                }

                for (Gpio *g : segment) {
                        *g = false;
                }
        }

        void setDigit (uint8_t position, uint8_t digit) override;

        uint8_t getDots () const override { return dots; }
        void setDots (uint8_t bitmask) override { dots = bitmask; }
        void setDot (uint8_t number, bool on) override;

        uint8_t getIcons () const override { return 0; }
        void setIcons (uint8_t /*bitmask*/) override {}

        void setTime (uint32_t) override;
        void setText (const char *) override;

        void setFlip (bool b) override { flip = b; }

        /**
         * @brief batteryLevel 0 : blinking frame, 1 : frame, 2 : frame & 1 bar, 3 : frame & 2 bars
         * @param level
         */
        void setBatteryLevel (uint8_t /*level*/) override {}
        void setBacklight (bool /*b*/) override {}
        bool getBacklight () const override { return true; }

        void setBrightness (uint8_t b) override { brightness = b % (MAX_BRIGHTNESS + 1); }
        uint8_t getBrightness () const override { return brightness; }

        void refresh () override;
        void clear () override;

        static constexpr size_t SPACE_CHAR = 36;

        static constexpr uint8_t fonts[] = {
                0b00111111, // 0
                0b00000110, // 1
                0b01011011, // 2
                0b01001111, // 3
                0b01100110, // 4
                0b01101101, // 5
                0b01111101, // 6
                0b00000111, // 7
                0b01111111, // 8
                0b01101111, // 9
                0b01110111, // A
                0b01111100, // b
                0b00111001, // C
                0b01011110, // d
                0b01111001, // E
                0b01110001, // F
                0b00111101, // G
                0b01110100, // h
                0b00000100, // i
                0b00011110, // j
                0b01110100, // k
                0b00111000, // L
                0b01010100, // m
                0b01010100, // n
                0b01011100, // o
                0b01110011, // P
                0b01100111, // q
                0b01010000, // r
                0b01101101, // S
                0b01111000, // t
                0b00011100, // u
                0b00011100, // v
                0b00011100, // w
                0b01110110, // x
                0b01100110, // y
                0b01011011, // z
                0b00000000, // [space] 36
        };

        void setResolution (Resolution res) override;

private:
        /// Turns a single display on or fof
        void turnDisplay (uint8_t d, bool b) { *common.at (d) = !b; }
        void outputDigit (uint8_t position);
        uint8_t flipFont (uint8_t font) { return (font & 0xc0) | (font & 0x07) << 3 | (font & 0x38) >> 3; }

private:
        static constexpr size_t DISPLAY_NUM = 6;
        std::array<Gpio *, 8> segment;
        std::array<Gpio *, DISPLAY_NUM> common;
        uint8_t digits[DISPLAY_NUM] = {0}; // TODO variable number of displays
        uint8_t dots = 0;
        uint8_t currentDigit = 0;

        static constexpr uint8_t MAX_BRIGHTNESS = 5;
        uint8_t brightness = MAX_BRIGHTNESS / 2;
        uint8_t brightnessCycle = 0;

        bool flip = false;
        Resolution resolution{};
        int factorIndex{};
        uint32_t prescaler = 1;
};
