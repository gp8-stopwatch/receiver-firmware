/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#ifdef WITH_FLASH
#include <storage/FlashEepromStorage.h>
#endif

extern unsigned int _config_storage_address;
extern unsigned int _hiscore_storage_address;
extern unsigned int _history_storage_address;

extern cfg::Config &getConfig ();

#ifdef WITH_FLASH
using ConfigFlashEepromStorage = FlashEepromStorage<2048, 2>;
extern ConfigFlashEepromStorage &getConfigFlashEepromStorage ();
#endif