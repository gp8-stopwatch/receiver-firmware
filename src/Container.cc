/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Container.h"

cfg::Config &getConfig ()
{
        static cfg::Config c;
        c = *reinterpret_cast<cfg::Config const *> (getConfigFlashEepromStorage ().read (nullptr, 2, 0, 0));
        return c;
}

ConfigFlashEepromStorage getConfigFlashEepromStorage ()
{
        static ConfigFlashEepromStorage o (2, 1, 0x08020000 - 4 * 2048);
        o.init ();
        return o;
}
