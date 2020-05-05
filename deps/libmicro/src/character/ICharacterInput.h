/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICHARACTERINPUT_H
#define ICHARACTERINPUT_H

#include "ICharacterSink.h"

struct ICharacterInput {
        virtual ~ICharacterInput () = default;
        virtual void setSink (ICharacterSink *s) = 0;
        virtual void pause () = 0;
        virtual void resume () = 0;
};

#endif // ICHARACTERINPUT_H
