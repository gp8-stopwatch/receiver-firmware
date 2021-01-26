/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "UsbHelpers.h"
#include "Debug.h"

void printResultS (Result time);
void printResultMs (Result time);

/*****************************************************************************/

void print (int i)
{
        std::array<char, 12> buf{};
        itoa (i, buf.data ());
        usbWrite (buf.data ());
}

/*****************************************************************************/

void print (unsigned int i)
{
        std::array<char, 12> buf{};
        itoa (i, buf.data ());
        usbWrite (buf.data ());
}

/****************************************************************************/

void printResult (Result time, ResultAccuracy ra)
{
        switch (ra) {
        case ResultAccuracy::SECOND:
                printResultS (time);
                break;
        case ResultAccuracy::MILISECOND:
                printResultMs (time);
                break;

        default:
                break;
        }
}

/****************************************************************************/

void printResultS (Result time)
{
        char buf[11];
        uint32_t sec100 = time % 100000;

        time /= 100000;
        uint32_t sec = time % 60;

        time /= 60;
        uint32_t min = time % 60;

        itoa ((unsigned int)(min), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(sec), buf, 2);
        usbWrite (buf);
        usbWrite (".");

        itoa ((unsigned int)(sec100), buf, 5);
        usbWrite (buf);
}

/****************************************************************************/

void printResultMs (Result time)
{
        char buf[11];
        uint32_t msecFrac = time % 100;
        uint32_t msec = time / 100;

        msec += (msecFrac >= 50) ? (1) : (0);
        itoa ((unsigned int)(msec), buf, 0);
        usbWrite (buf);
}

/*****************************************************************************/

void printDate (RTC_DateTypeDef const &date, Time const &time)
{
        char buf[11];
        itoa ((unsigned int)(date.Year + 2000), buf, 4);
        usbWrite (buf);
        usbWrite ("-");

        itoa ((unsigned int)(date.Month), buf, 2);
        usbWrite (buf);
        usbWrite ("-");

        itoa ((unsigned int)(date.Date), buf, 2);
        usbWrite (buf);
        usbWrite (" ");

        itoa ((unsigned int)(time.Hours), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(time.Minutes), buf, 2);
        usbWrite (buf);
        usbWrite (":");

        itoa ((unsigned int)(time.Seconds), buf, 2);
        usbWrite (buf);
}
