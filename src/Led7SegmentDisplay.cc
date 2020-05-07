/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Led7SegmentDisplay.h"
#include <array>

/*****************************************************************************/

void Led7SegmentDisplay::refresh ()
{
        if (!brightness) {
                return;
        }

        // Change a digit at the beginning of each "brightnessCycle" and lit it up.
        if (brightnessCycle == 0) {
                turnDisplay (currentDigit, false);
                ++currentDigit;
                currentDigit %= 6;
                outputDigit (currentDigit);
                turnDisplay (currentDigit, true);
        }

        if (brightnessCycle >= brightness) {
                turnDisplay (currentDigit, false);
        }

        ++brightnessCycle;
        brightnessCycle %= MAX_BRIGHTNESS + 1;
}

/*****************************************************************************/

void Led7SegmentDisplay::setDigit (uint8_t position, uint8_t digit)
{
        if (digit >= 0 && digit <= 0xf) {
                digits[position] = digit;
        }
        else if (digit >= 48 && digit <= 57) {
                digits[position] = digit - 48;
        }
        else if (digit >= 65 /*A*/ && digit <= 90 /*Z*/) {
                digits[position] = digit - 55;
        }
        else if (digit >= 97 /*a*/ && digit <= 122 /*z*/) {
                digits[position] = digit - 87;
        }
        else if (digit == '.') {
                setDot (position, true);
        }
        else if (digit == ' ') {
                digits[position] = SPACE_CHAR;
        }
}

/*****************************************************************************/

void Led7SegmentDisplay::outputDigit (uint8_t position)
{
        if (flip) {
                position = DISPLAY_NUM - 1 - position;
        }

        uint8_t font = fonts[digits[position]];

        if (flip) {
                font = flipFont (font);
        }

        for (uint8_t seg = 0; seg < 7; ++seg) {
                *segment.at (seg) = bool (font & (1 << seg));
        }

        *segment.at (7) = dots & (1 << position);
}

/*****************************************************************************/

void Led7SegmentDisplay::setTime (uint32_t time)
{
        unsigned int cntTmp = time;
        auto factorIndexCopy = factorIndex;

        for (int i = 5; i >= 0; --i, ++factorIndexCopy) {
                auto factor = FACTORS.at (factorIndexCopy);
                setDigit (i, cntTmp % factor);
                cntTmp /= factor;
        }

        // // 2nd digit of 1/100-s of second (0-99)
        // setDigit (5, cntTmp % 10);
        // cntTmp /= 10;
        // // First digit of 1/100-s of second (0-99)
        // setDigit (4, cntTmp % 10);
        // cntTmp /= 10;

        // // Second digit of second (0-99)
        // setDigit (3, cntTmp % 10);
        // cntTmp /= 10;
        // // First digit of second (0-99)
        // setDigit (2, cntTmp % 6);
        // cntTmp /= 6;

        // // Second digit of miniutes
        // setDigit (1, cntTmp % 10);
        // cntTmp /= 10;
        // setDigit (0, cntTmp);
}

/*****************************************************************************/

void Led7SegmentDisplay::setText (const char *s)
{
        std::string_view txt (s);

        uint8_t i = 0;
        for (size_t j = 0; j < txt.size (); ++j) {
                char c = txt.at (j);
                setDigit (i, c);

                if (j + 1 < txt.size () && txt.at (j + 1) != '.') {
                        ++i;
                }

                if (i >= DISPLAY_NUM) {
                        break;
                }
        }
}

/*****************************************************************************/

void Led7SegmentDisplay::setDot (uint8_t number, bool on)
{
        if (on) {
                dots |= (1 << number);
        }
        else {
                dots &= ~(1 << number);
        }
}

/*****************************************************************************/

void Led7SegmentDisplay::clear ()
{
        for (size_t i = 0; i < DISPLAY_NUM; ++i) {
                digits[i] = SPACE_CHAR;
        }

        dots = 0;
}

/*****************************************************************************/

void Led7SegmentDisplay::setResolution (Resolution res)
{
        resolution = res;

        switch (res) {
        case Resolution::ms_10:
                factorIndex = 2; // 100Hz
                break;

        case Resolution::ms_1:
                factorIndex = 1; // 1kHz
                break;

        case Resolution::ms_01:
                factorIndex = 0; // 10kHz
                break;

        default:
                break;
        }
}
