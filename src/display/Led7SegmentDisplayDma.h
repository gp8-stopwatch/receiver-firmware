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

private:
#ifdef COMMON_ANODE
        static constexpr bool CA = true;
#else
        static constexpr bool CA = false;
#endif

        static constexpr int DISPLAY_NUM = 6;
        uint8_t dots = 0;
        // uint8_t currentDigit = 0;

        static constexpr uint8_t MAX_BRIGHTNESS = 4;
        uint8_t brightness{};
        // uint8_t brightnessCycle = 0;

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

        static constexpr uint32_t ENABLE0_MASK = (1 << (ENABLE5_PIN_NUM + 16)) | (1 << ENABLE0_PIN_NUM);
        static constexpr uint32_t ENABLE1_MASK = (1 << (ENABLE0_PIN_NUM + 16)) | (1 << ENABLE1_PIN_NUM);
        static constexpr uint32_t ENABLE2_MASK = (1 << (ENABLE1_PIN_NUM + 16)) | (1 << ENABLE2_PIN_NUM);
        static constexpr uint32_t ENABLE3_MASK = (1 << (ENABLE2_PIN_NUM + 16)) | (1 << ENABLE3_PIN_NUM);
        static constexpr uint32_t ENABLE4_MASK = (1 << (ENABLE3_PIN_NUM + 16)) | (1 << ENABLE4_PIN_NUM);
        static constexpr uint32_t ENABLE5_MASK = (1 << (ENABLE4_PIN_NUM + 16)) | (1 << ENABLE5_PIN_NUM);

        static constexpr uint32_t ALL_ENABLE_OFF = (1 << (ENABLE5_PIN_NUM + 16)) | (1 << (ENABLE0_PIN_NUM + 16)) | (1 << (ENABLE1_PIN_NUM + 16))
                | (1 << (ENABLE2_PIN_NUM + 16)) | (1 << (ENABLE3_PIN_NUM + 16)) | (1 << (ENABLE4_PIN_NUM + 16));

        static constexpr std::array ENABLE_MASKS = {
                ENABLE5_MASK, // enable display 5 (PB5)
                ENABLE0_MASK, // enable display 0 (PB11)
                ENABLE1_MASK, // enable display 1 (PB12) shifted due to timers synchronization
                ENABLE2_MASK, // enable display 2 (PB13)
                ENABLE3_MASK, // enable display 3 (PB10)
                ENABLE4_MASK, // enable display 4 (PB2)
        };

        std::array<uint32_t, DISPLAY_NUM * MAX_BRIGHTNESS> enableBuffer{
                ENABLE5_MASK, ENABLE5_MASK, ENABLE5_MASK, ENABLE0_MASK, ENABLE0_MASK, ENABLE0_MASK, ENABLE0_MASK, ENABLE1_MASK, ENABLE1_MASK,
                ENABLE1_MASK, ENABLE1_MASK, ENABLE2_MASK, ENABLE2_MASK, ENABLE2_MASK, ENABLE2_MASK, ENABLE3_MASK, ENABLE3_MASK, ENABLE3_MASK,
                ENABLE3_MASK, ENABLE4_MASK, ENABLE4_MASK, ENABLE4_MASK, ENABLE4_MASK, ENABLE5_MASK

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
