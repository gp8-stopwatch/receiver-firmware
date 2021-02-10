/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Container.h"
#include "Gpio.h"

namespace {
// Hack to be able to pass the cli object pointer to the C-like function.
// void *cliPointer{};

bool showGreeting{};
} // namespace

/****************************************************************************/

Led7SegmentDisplay &getDisplay ()
{
#ifdef WITH_DISPLAY
        static Gpio d1 (GPIOB, GPIO_PIN_11);
        static Gpio d2 (GPIOB, GPIO_PIN_12);
        static Gpio d3 (GPIOB, GPIO_PIN_13);
        static Gpio d4 (GPIOB, GPIO_PIN_10);
        static Gpio d5 (GPIOB, GPIO_PIN_2);
        static Gpio d6 (GPIOA, GPIO_PIN_5);

        static Gpio sa (GPIOA, GPIO_PIN_0);
        static Gpio sb (GPIOA, GPIO_PIN_1);
        static Gpio sc (GPIOA, GPIO_PIN_2);
        static Gpio sd (GPIOA, GPIO_PIN_3);
        static Gpio se (GPIOA, GPIO_PIN_6);
        static Gpio sf (GPIOA, GPIO_PIN_7);
        static Gpio sg (GPIOB, GPIO_PIN_5);
        static Gpio sdp (GPIOB, GPIO_PIN_1);

#if 0
        // !!!WARNING!!!
        // Uncomenting this will pass 100mA continuous through all displays and can damage them.
        d1 = d2 = d3 = d4 = d5 = d6 = false;
        sa = sb = sc = sd = se = sf = sg = sdp = true;

        while (true) {
        }
#endif

#ifdef PLATFORM_HUGE
        // TODO remove this when new PCBs for huge diplay are ordered
        static Led7SegmentDisplay display (sa, sb, sc, sd, se, sf, sg, sdp, d3, d2, d1, d4, d5, d6);
#else
        static Led7SegmentDisplay display (sa, sb, sc, sd, se, sf, sg, sdp, d1, d2, d3, d4, d5, d6);
#endif

#else
        static FakeDisplay display;
#endif

#if 0
        display.setDigit (0, 0xa);
        display.setDigit (1, 0xb);
        display.setDigit (2, 0xc);
        display.setDigit (3, 0xd);
        display.setDigit (4, 0xe);
        display.setDigit (5, 0xf);

        while (true) {
        }
#endif

        return display;
}

/****************************************************************************/

void readConfigFromFlash ()
{
#ifdef WITH_FLASH
        getConfig () = *reinterpret_cast<cfg::Config const *> (getConfigFlashEepromStorage ().read (nullptr, 2, 0, 0));
#endif
}

/****************************************************************************/

cfg::Config &getConfig ()
{
        static cfg::Config c;
        return c;
}

/****************************************************************************/

#ifdef WITH_FLASH
ConfigFlashEepromStorage &getConfigFlashEepromStorage ()
{
        static ConfigFlashEepromStorage o (4, 1, size_t (&_config_storage_address));
        // o.init ();
        return o;
}
#endif

/****************************************************************************/

auto &getIrTriggerInput ()
{
        // IR on means the state is LOW. Beam interruption means transition from LOW to HI i.e. rising.
        static Gpio irTriggerInput (IR_PORT, IR_PINS, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL); // TODO GPIO_PULLDOWN & test
        return irTriggerInput;
}

/****************************************************************************/

auto &getExtTriggerInput ()
{
        // External trigger
        static Gpio extTriggerInput (EXT_TRIGGER_INPUT_PORT, EXT_TRIGGER_INPUT_PINS, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLDOWN);
        return extTriggerInput;
}

/****************************************************************************/

InfraRedBeamExti &getBeam ()
{
        static Gpio extTriggerOutput (EXT_TRIGGER_OUTPUT_PORT, EXT_TRIGGER_OUTPUT_PINS, GPIO_MODE_OUTPUT_PP);
        static Gpio extTriggerOutEnable (EXT_TRIGGER_OUT_ENABLE_PORT, EXT_TRIGGER_OUT_ENABLE_PINS, GPIO_MODE_OUTPUT_PP);
        static InfraRedBeamExti beam{getIrTriggerInput (), extTriggerOutput, extTriggerOutEnable};
        return beam;
}

/****************************************************************************/

Can &getCan ()
{
#ifdef WITH_CAN
        static Gpio canGpio1 (CAN_PORT_1, CAN_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, CAN_ALTERNATE);
        static Gpio canGpio2 (CAN_PORT_2, CAN_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, CAN_ALTERNATE);

        // 24 - 125kbps, 60 : 50kbps
        static Can can (nullptr, 60, CAN_SJW_3TQ, CAN_BS1_12TQ, CAN_BS2_3TQ);
        return can;
#endif
}

/****************************************************************************/

CanProtocol &getProtocol ()
{
        static CanProtocol protocol (getCan (), *MICRO_CONTROLLER_UID, myDeviceType);
        return protocol;
}

/****************************************************************************/

Rtc &getRtc ()
{
#ifdef WITH_RTC
        static Rtc rtc;
        return rtc;
#endif
}

/****************************************************************************/

History &getHistory ()
{
#ifdef WITH_HISTORY
        static History history{getRtc ()};
        static FlashEepromStorage<2048, 4> hiScoreStorage (4, 1, size_t (&_hiscore_storage_address));
        hiScoreStorage.init ();
        history.setHiScoreStorage (&hiScoreStorage);

        static FlashEepromStorage<2048, 4> historyStorage (12, 2, size_t (&_history_storage_address));
        historyStorage.init ();
        history.setHistoryStorage (&historyStorage);
        return history;
#endif
}

/****************************************************************************/

Buzzer &getBuzzer ()
{
#ifdef WITH_SOUND
        static Gpio buzzerPin (GPIOB, GPIO_PIN_14);
        static Buzzer buzzer (buzzerPin);

        if (getConfig ().isBuzzerOn ()) {
                buzzer.beep (20, 0, 1);
        }

        return buzzer;
#endif
}

/****************************************************************************/

StopWatch &getStopWatch ()
{
        static StopWatch s;
        return s;
}

/****************************************************************************/

FastStateMachine &getFastStateMachine ()
{
        static FastStateMachine fStateMachine{};
        fStateMachine.setStopWatch (&getStopWatch ());
        return fStateMachine;
}

/****************************************************************************/

Button &getButton ()
{
#ifdef WITH_BUTTON
        static Gpio buttonPin (GPIOB, GPIO_PIN_15, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL);
        static Button button (buttonPin);
        return button;
#endif
}

/****************************************************************************/

PowerManagement &getPowerManager ()
{
#ifdef WITH_POWER_MANAGER
        static PowerManagement power{getDisplay (), getFastStateMachine ()};
        return power;
#endif
}

/****************************************************************************/

DisplayMenu &getMenu ()
{
        static DisplayMenu menu (getConfig (), getDisplay (), getFastStateMachine ());

#ifdef WITH_HISTORY
        menu.setHistory (&getHistory ());
#endif

#ifdef WITH_RTC
        menu.setRtc (&getRtc ());
#endif
        return menu;
}

/****************************************************************************/

namespace container {

void init ()
{
        static bool initDone{};

        if (initDone) {
                return;
        }

        /*+-------------------------------------------------------------------------+*/
        /*| Screen                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        static HardwareTimer tim15 (TIM15, 48 - 1, 200 - 1); // Update 5kHz
        HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (TIM15_IRQn);
        tim15.setOnUpdate ([] { getDisplay ().refresh (); });

        /*+-------------------------------------------------------------------------+*/
        /*| Config                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        getConfig ();
#ifdef WITH_FLASH
        getConfigFlashEepromStorage ().init (); // TODO use RAII
        readConfigFromFlash ();
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Debug UART                                                              |*/
        /*+-------------------------------------------------------------------------+*/

        static Gpio debugUartGpios (DEBUG_PORT, DEBUG_PINS, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, DEBUG_ALTERNATE);
        static Usart debugUart (DEBUG_UART, 115200);

        static Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();

        /*+-------------------------------------------------------------------------+*/
        /*| CAN                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        HAL_NVIC_SetPriority (CEC_CAN_IRQn, CAN_BUS_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (CEC_CAN_IRQn);
        getProtocol ();
        getCan ().setCanCallback (&getProtocol ());
        getCan ().setFilterAndMask (0x00000000, 0x00000000, true);
        getCan ().interrupts (true, false);

        /*+-------------------------------------------------------------------------+*/
        /*| RTC                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        getRtc ();

        /*+-------------------------------------------------------------------------+*/
        /*| History saved in the flash                                              |*/
        /*+-------------------------------------------------------------------------+*/

        getHistory ();

        /*+-------------------------------------------------------------------------+*/
        /*| Backlight, beeper                                                       |*/
        /*+-------------------------------------------------------------------------+*/

        getBuzzer ();

        /*+-------------------------------------------------------------------------+*/
        /*| StopWatch, machine and IR                                               |*/
        /*+-------------------------------------------------------------------------+*/

        getStopWatch ();
        getFastStateMachine ();

        /*+-------------------------------------------------------------------------+*/
        /*| Light barier beam                                                       |*/
        /*+-------------------------------------------------------------------------+*/

        getIrTriggerInput ().setOnToggle ([] { getBeam ().onExti (getBeam ().getPinState (), false); });
        getExtTriggerInput ().setOnToggle (
                [] { getBeam ().onExti ((getExtTriggerInput ().get ()) ? (IrBeam::absent) : (IrBeam::present), true); });

        getProtocol ().setBeam (&getBeam ());
        getBeam ().setFastStateMachine (&getFastStateMachine ());
        getBeam ().setStopWatch (&getStopWatch ());

#ifdef WITH_CAN
        static FastStateMachineProtocolCallback callback{getFastStateMachine ()};
        getProtocol ().setCallback (&callback);
#endif
        getFastStateMachine ().setIr (&getBeam ());
        getFastStateMachine ().setDisplay (&getDisplay ());

#ifdef WITH_SOUND
        getFastStateMachine ().setBuzzer (&getBuzzer ());
#endif

#ifdef WITH_HISTORY
        getFastStateMachine ().setHistory (&getHistory ());
#endif

#ifdef WITH_CAN
        getFastStateMachine ().setCanProtocol (&getProtocol ());
#endif

        HAL_NVIC_SetPriority (IR_IRQn, IR_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (IR_IRQn);

        HAL_NVIC_SetPriority (EXT_TRIGGER_INPUT_IRQn, EXT_TRIGGER_INPUT_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (EXT_TRIGGER_INPUT_IRQn);

        /*+-------------------------------------------------------------------------+*/
        /*| Button                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        HAL_NVIC_SetPriority (BUTTON_IRQn, BUTTON_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (BUTTON_IRQn);

        /*+-------------------------------------------------------------------------+*/
        /*| Battery, light sensor, others                                            |*/
        /*+-------------------------------------------------------------------------+*/

        getPowerManager ();

        /*+-------------------------------------------------------------------------+*/
        /*| Menu                                                                    |*/
        /*+-------------------------------------------------------------------------+*/

        getMenu ();

        /*+-------------------------------------------------------------------------+*/
        /*| USB                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        usbcli::init ();
}

} // namespace container