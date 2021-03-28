/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Button.h"
#include "Buzzer.h"
#include "Can.h"
#include "CanProtocol.h"
#include "Config.h"
#include "Container.h"
#include "Debug.h"
#include "DisplayMenu.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"
#include "Gpio.h"
#include "HardwareTimer.h"
#include "History.h"
#include "Led7SegmentDisplay.h"
#include "Led7SegmentDisplayDma.h"
#include "PowerManagement.h"
#include "Rtc.h"
#include "StopWatch.h"
#include "Timer.h"
#include "Types.h"
#include "Usart.h"
#include "UsbCli.h"
#include "detector/Detector.h"
#include <cstdbool>
#include <cstring>
#include <etl/string.h>
#include <limits>
#include <new>
#include <stm32f0xx_hal.h>

#ifdef WITH_FLASH
#include <storage/FlashEepromStorage.h>
#endif

extern unsigned int _config_storage_address;
extern unsigned int _hiscore_storage_address;
extern unsigned int _history_storage_address;
static const uint32_t *const MICRO_CONTROLLER_UID = new (reinterpret_cast<void *> (0x1FFFF7AC)) uint32_t;

namespace container {
extern void init ();
} // namespace container

extern IDisplay &getDisplay ();
extern cfg::Config &getConfig ();
extern void readConfigFromFlash ();

#ifdef WITH_FLASH
using ConfigFlashEepromStorage = FlashEepromStorage<2048, 2>;
extern ConfigFlashEepromStorage &getConfigFlashEepromStorage ();
#endif

extern EventQueue eventQueue;

// extern InfraRedBeamExti &getBeam ();
extern DisplayMenu &getMenu ();
extern Buzzer &getBuzzer ();
extern Button &getButton ();
extern History &getHistory ();
extern PowerManagement &getPowerManager ();
extern StopWatch &getStopWatch ();
extern FastStateMachine &getFastStateMachine ();
extern Rtc &getRtc ();
extern CanProtocol &getProtocol ();
extern EdgeFilter &getIrDetector ();
extern EdgeFilter &getExtDetector ();
extern Gpio &getIrTriggerInput ();
extern Gpio &getExtTriggerInput ();