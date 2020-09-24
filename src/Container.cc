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
        return c;
}

#ifdef WITH_FLASH
ConfigFlashEepromStorage &getConfigFlashEepromStorage ()
{
        static ConfigFlashEepromStorage o (4, 1, size_t (&_config_storage_address));
        o.init ();
        return o;
}
#endif