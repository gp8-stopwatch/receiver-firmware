/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include <storage/FlashEepromStorage.h>

extern cfg::Config &getConfig ();

using ConfigFlashEepromStorage = FlashEepromStorage<2048, 2>;
extern ConfigFlashEepromStorage getConfigFlashEepromStorage ();