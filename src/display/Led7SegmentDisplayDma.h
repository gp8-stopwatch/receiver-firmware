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

#if defined(COMMON_ANODE) && defined(COMMON_CATHODE)
#error "Both COMMON_ANODE and COMMON_CATHODE macros cannot be defined at the same time"
#endif

#if !defined(COMMON_ANODE) && !defined(COMMON_CATHODE)
#error "Define either COMMON_ANODE or COMMON_CATHODE macro"
#endif

/**
 * See AN4666 Application note. Parallel synchronous transmission using GPIO and DMA
 * Important : for this "driver" to work, all the segmnents have to be connected to the
 * same port, and all the commons (CAs or CCs) have to be connected to a single port.
 * For example now all segments are connected to PAx, and commons to PBx.
 */
class Led7SegmentDisplayDma : public IDisplay {
public:
        Led7SegmentDisplayDma ();

        void setDigit (uint8_t position, uint8_t digit) override;

        uint8_t getDots () const override { return dots; }
        void setDots (uint8_t bitmask) override { dots = bitmask; }
        void setDot (uint8_t number, bool on) override;

        uint8_t getIcons () const override { return 0; }
        void setIcons (uint8_t /*bitmask*/) override {}

        void setTime (Result10us time, Resolution res) override;
        void setText (const char *s) override;

        void setFlip (bool b) override { flip = b; }

        /**
         * @brief batteryLevel 0 : blinking frame, 1 : frame, 2 : frame & 1 bar, 3 : frame & 2 bars
         * @param level
         */
        void setBatteryLevel (uint8_t /*level*/) override {}
        void setBacklight (bool /*b*/) override {}
        bool getBacklight () const override { return true; }

        void setBrightness (uint8_t b) override { brightness = std::min<uint8_t> (MAX_BRIGHTNESS, b); }
        uint8_t getBrightness () const override { return brightness; }

        void clear () override;

        static constexpr size_t SPACE_CHAR_INDEX = 36;

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

        static constexpr std::array<uint16_t, 10> FONTS{
                //        p feg  dcba
                0b0000'0000'0010'0000, // 0
                0b0000'0000'1110'1001, // 1
                0b0000'0000'1000'0100, // 2
                0b0000'0000'1100'0000, // 3
                0b0000'0000'0100'0001, // 4
                0b0000'0000'0100'0010, // 5
                0b0000'0000'0000'0010, // 6
                0b0000'0000'0110'1000, // 7
                0b0000'0000'0000'0000, // 8
                0b0000'0000'0100'0000, // 9

                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
                // 0b0000'0000'0000'0000, // 0
        };

        void setResolution (Resolution res) override;

private:
        uint8_t flipFont (uint8_t font) { return (font & 0xc0) | (font & 0x07) << 3 | (font & 0x38) >> 3; }

private:
#ifdef COMMON_ANODE
        static constexpr bool CA = true;
#else
        static constexpr bool CA = false;
#endif

        static constexpr size_t DISPLAY_NUM = 6;
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

        //           p feg  dcba
        // 0b0000'0001'1110'1111

        static constexpr uint32_t ALL_SEGMENTS = 0b0000'0001'1110'1111'0000'0000'0000'0000;
        static constexpr uint32_t NO_SEGMENTS = 0b0000'0000'0000'0000'0000'0001'1110'1111;

        std::array<uint32_t, DISPLAY_NUM> displayBuffer{

        };

        const std::array<uint32_t, DISPLAY_NUM> enableBuffer{
                0b0000'0000'0010'0000'0000'1000'0000'0000, //
                0b0000'1000'0000'0000'0001'0000'0000'0000, //
                0b0001'0000'0000'0000'0010'0000'0000'0000, //
                0b0010'0000'0000'0000'0000'0100'0000'0000, //
                0b0000'0100'0000'0000'0000'0000'0000'0100, //
                0b0000'0000'0000'0100'0000'0000'0010'0000, //
        };
};
