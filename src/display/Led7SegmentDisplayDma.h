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

extern "C" void DMA1_Channel2_3_IRQHandler ();

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

        void setFlip (bool b) override;

        /**
         * @brief batteryLevel 0 : blinking frame, 1 : frame, 2 : frame & 1 bar, 3 : frame & 2 bars
         * @param level
         */
        void setBatteryLevel (uint8_t /*level*/) override {}
        void setBacklight (bool /*b*/) override {}
        bool getBacklight () const override { return true; }

        void setBrightness (uint8_t b) override;
        uint8_t getBrightness () const override { return brightness; }

        void clear () override;

        void setResolution (Resolution res) override;

        void setFps (unsigned int fps) override;

private:
#ifdef COMMON_ANODE
        static constexpr bool CA = true;
#else
        static constexpr bool CA = false;
#endif

        friend void DMA1_Channel2_3_IRQHandler ();

        void init (uint16_t fps);

        static constexpr int DISPLAY_NUM = 6;
        static constexpr uint16_t calculatePeriod (uint32_t fps) { return 1000000 / (fps * 24); } // TODO why 24 instead of 12?
        static constexpr uint16_t PRESCALER = 48;                                                 // CPU runs @ 48MHz

        uint8_t dots = 0;

        static constexpr uint8_t MIN_BRIGHTNESS = 1;
        static constexpr uint8_t MAX_BRIGHTNESS = 4;
        uint8_t brightness{};
        uint8_t prevBrightness{};
        std::array<uint16_t, MAX_BRIGHTNESS> brightnessLookup{};

        void recalculateBrightnessTable (unsigned int fps);
        uint16_t fps{};
        uint16_t prevFps{};

        bool flip = false;
        Resolution resolution{};
        int factorIndex{};
        uint32_t prescaler = 1;

        static constexpr uint32_t ALL_SEGMENTS = 0b0000'0001'1110'1111'0000'0000'0000'0000;
        static constexpr uint32_t NO_SEGMENTS = 0b0000'0000'0000'0000'0000'0001'1110'1111;
        static constexpr uint16_t DOT_MASK = 0b0000'0001'0000'0000;
        static constexpr size_t SPACE_CHAR_INDEX = 36;

        std::array<uint32_t, DISPLAY_NUM> displayBuffer{};

        static constexpr auto ENABLE0_PIN_NUM = 11;
        static constexpr auto ENABLE1_PIN_NUM = 12;
        static constexpr auto ENABLE2_PIN_NUM = 13;
        static constexpr auto ENABLE3_PIN_NUM = 10;
        static constexpr auto ENABLE4_PIN_NUM = 2;
        static constexpr auto ENABLE5_PIN_NUM = 5;

        static constexpr uint32_t ENABLE0_ON = (1 << ENABLE0_PIN_NUM);
        static constexpr uint32_t ENABLE1_ON = (1 << ENABLE1_PIN_NUM);
        static constexpr uint32_t ENABLE2_ON = (1 << ENABLE2_PIN_NUM);
        static constexpr uint32_t ENABLE3_ON = (1 << ENABLE3_PIN_NUM);
        static constexpr uint32_t ENABLE4_ON = (1 << ENABLE4_PIN_NUM);
        static constexpr uint32_t ENABLE5_ON = (1 << ENABLE5_PIN_NUM);

        static constexpr uint32_t ALL_ENABLE_OFF = (1 << (ENABLE5_PIN_NUM + 16)) | (1 << (ENABLE0_PIN_NUM + 16)) | (1 << (ENABLE1_PIN_NUM + 16))
                | (1 << (ENABLE2_PIN_NUM + 16)) | (1 << (ENABLE3_PIN_NUM + 16)) | (1 << (ENABLE4_PIN_NUM + 16));

        std::array<uint32_t, DISPLAY_NUM * 2> enableBuffer{
                ENABLE4_ON, ALL_ENABLE_OFF, ENABLE5_ON, ALL_ENABLE_OFF, ENABLE0_ON, ALL_ENABLE_OFF,
                ENABLE1_ON, ALL_ENABLE_OFF, ENABLE2_ON, ALL_ENABLE_OFF, ENABLE3_ON, ALL_ENABLE_OFF,

        };

        /// Corresponds to DSEG7 font as found on https://www.keshikan.net/fonts-e.html
        static constexpr std::array<uint16_t, 37> FONTS{
                //        p feg  dcba
                0b0000'0001'0010'0000, // 0
                0b0000'0001'1110'1001, // 1
                0b0000'0001'1000'0100, // 2
                0b0000'0001'1100'0000, // 3
                0b0000'0001'0100'1001, // 4
                0b0000'0001'0100'0010, // 5
                0b0000'0001'0000'0010, // 6
                0b0000'0001'0110'1000, // 7
                0b0000'0001'0000'0000, // 8
                0b0000'0001'0100'0000, // 9

                0b0000'0001'0000'1000, // A
                0b0000'0001'0000'0011, // b
                0b0000'0001'0010'0110, // C
                0b0000'0001'1000'0001, // d
                0b0000'0001'0000'0110, // E
                0b0000'0001'0000'1110, // F
                0b0000'0001'0010'0010, // G
                0b0000'0001'0000'1001, // H
                0b0000'0001'1110'1011, // i
                0b0000'0001'1010'0001, // J
                0b0000'0001'0000'1010, // K
                0b0000'0001'0010'0111, // L
                0b0000'0001'0010'1000, // M
                0b0000'0001'1000'1011, // n
                0b0000'0001'1000'0011, // o
                0b0000'0001'0000'1100, // P
                0b0000'0001'0100'1000, // q
                0b0000'0001'1000'1111, // r
                0b0000'0001'0100'0011, // S
                0b0000'0001'0000'0111, // t
                0b0000'0001'1010'0011, // u
                0b0000'0001'0010'0001, // V
                0b0000'0001'0000'0001, // W
                0b0000'0001'0000'1001, // X
                0b0000'0001'0100'0001, // Y
                0b0000'0001'1010'0100, // Z

                NO_SEGMENTS // Space 36
        };
};
