/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef __DEBUG_DRIVER_H__
#define __DEBUG_DRIVER_H__

#include <Hal.h>
#include <cstdint>
#include <cstdlib>

class Usart;

class Debug {
public:
        static Debug *&singleton ();

        Debug (Usart *uart) : uart (uart) {}

        void print (const char *str);
        void println (const char *str);

        void print (uint8_t const *data, size_t len);
        void println (uint8_t const *data, size_t len);

        void printArray (uint8_t const *data, size_t len);

        void print (int);
        void println (int);

        void printTime (uint16_t time);

        /*---------------------------------------------------------------------------*/

        //        static void print (const char *str) { singleton ()->print (str); }
        //        static void println (const char *str) { singleton ()->println (str); }

        //        static void print (uint8_t *data, size_t len) { singleton ()->print (data, len); }
        //        static void printArray (uint8_t *data, size_t len) { singleton ()->printArray (data, len); }

        //        static void print (int i) { singleton ()->print (i); }
        //        static void println (int i) { singleton ()->print (i); }

        //        static void printTime (uint16_t time) { singleton ()->print (time); }

public:
        Usart *uart;
        static Debug *instance;
};

// TODO hack, get rid of this.
extern Debug *debug;

#endif //__CMD_UART_H__
