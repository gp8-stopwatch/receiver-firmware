/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICHARACTEROUTPUT_H
#define ICHARACTEROUTPUT_H

#include <cstdlib>

struct ICharacterOutput {
        virtual ~ICharacterOutput () {}
        virtual void transmit (const char *str) = 0;
        virtual void transmit (const char *str, size_t len) = 0;
};

#endif // ICHARACTEROUTPUT_H
