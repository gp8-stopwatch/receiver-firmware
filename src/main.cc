/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Button.h"
#include "Buzzer.h"
#include "Can.h"
#include "CanProtocol.h"
#include "Cli.h"
#include "Container.h"
#include "Debug.h"
#include "DisplayMenu.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"
#include "Gpio.h"
#include "HardwareTimer.h"
#include "History.h"
#include "InfraRedBeamExti.h"
#include "Led7SegmentDisplay.h"
#include "PowerManagement.h"
#include "Rtc.h"
#include "StopWatch.h"
#include "Timer.h"
#include "Types.h"
#include "Usart.h"
#include "usb/UsbHelpers.h"
#include "usbd_cdc.h"
#include "usbd_composite.h"
#include "usbd_desc.h"
#include <cstdbool>
#include <cstring>
#include <etl/string.h>
#include <limits>
#include <new>
#include <stm32f0xx_hal.h>

static void SystemClock_Config ();
USBD_HandleTypeDef USBD_Device{};

/*****************************************************************************/

namespace cl {

template <> struct Traits<String> {
        static constexpr LineEnd outputLineEnd{LineEnd::crlf};
        static constexpr size_t maxTokenSize = 16;
        static constexpr bool echo = true;
        static constexpr Case comparison = Case::insensitive;
};

template <> void output<String> (String const &tok) { usbWrite (tok.c_str ()); }
template <> void output<char> (char const &tok) { usbWriteData (reinterpret_cast<uint8_t const *> (&tok), 1); }
template <> void output<const char *> (const char *const &tok) { usbWrite (tok); }

} // namespace cl

namespace {
// Hack to be able to pass the cli object pointer to the C-like function.
void *cliPointer{};
bool showGreeting{};
} // namespace

void readConfigFromFlash ()
{
#ifdef WITH_FLASH
        getConfig () = *reinterpret_cast<cfg::Config const *> (getConfigFlashEepromStorage ().read (nullptr, 2, 0, 0));
#endif
}

/*****************************************************************************/

int main ()
{
        __HAL_RCC_SYSCFG_CLK_ENABLE ();
        __HAL_RCC_PWR_CLK_ENABLE ();

#ifdef PLATFORM_MICRO
        // ensure the kit is correct if this does not work.
        __HAL_REMAP_PIN_ENABLE (HAL_REMAP_PA11_PA12);
#endif

        HAL_Init ();
        SystemClock_Config ();

        /*+-------------------------------------------------------------------------+*/
        /*| Screen                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_DISPLAY
        Gpio d1 (GPIOB, GPIO_PIN_11);
        Gpio d2 (GPIOB, GPIO_PIN_12);
        Gpio d3 (GPIOB, GPIO_PIN_13);
        Gpio d4 (GPIOB, GPIO_PIN_10);
        Gpio d5 (GPIOB, GPIO_PIN_2);
        Gpio d6 (GPIOA, GPIO_PIN_5);

        Gpio sa (GPIOA, GPIO_PIN_0);
        Gpio sb (GPIOA, GPIO_PIN_1);
        Gpio sc (GPIOA, GPIO_PIN_2);
        Gpio sd (GPIOA, GPIO_PIN_3);
        Gpio se (GPIOA, GPIO_PIN_6);
        Gpio sf (GPIOA, GPIO_PIN_7);
        Gpio sg (GPIOB, GPIO_PIN_5);
        Gpio sdp (GPIOB, GPIO_PIN_1);

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
        Led7SegmentDisplay display (sa, sb, sc, sd, se, sf, sg, sdp, d3, d2, d1, d4, d5, d6);
#else
        Led7SegmentDisplay display (sa, sb, sc, sd, se, sf, sg, sdp, d1, d2, d3, d4, d5, d6);
#endif

        HardwareTimer tim15 (TIM15, 48 - 1, 200 - 1); // Update 5kHz
        HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (TIM15_IRQn);

        tim15.setOnUpdate ([&display] { display.refresh (); });
#else
        FakeDisplay display;
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
        /*+-------------------------------------------------------------------------+*/
        /*| Config                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        const uint32_t *MICRO_CONTROLLER_UID = new (reinterpret_cast<void *> (0x1FFFF7AC)) uint32_t;
        cfg::Config &config = getConfig ();
#ifdef WITH_FLASH
        getConfigFlashEepromStorage ().init (); // TODO use RAII
        readConfigFromFlash ();
#endif

        Gpio debugUartGpios (DEBUG_PORT, DEBUG_PINS, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, DEBUG_ALTERNATE);
        Usart debugUart (DEBUG_UART, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();

        /*+-------------------------------------------------------------------------+*/
        /*| CAN                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_CAN
        Gpio canGpio1 (CAN_PORT_1, CAN_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, CAN_ALTERNATE);
        Gpio canGpio2 (CAN_PORT_2, CAN_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, CAN_ALTERNATE);

        // 24 - 125kbps
        Can can (nullptr, 24, CAN_SJW_3TQ, CAN_BS1_12TQ, CAN_BS2_3TQ);
        HAL_NVIC_SetPriority (CEC_CAN_IRQn, CAN_BUS_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (CEC_CAN_IRQn);

        CanProtocol protocol (can, *MICRO_CONTROLLER_UID, myDeviceType);
        can.setCanCallback (&protocol);
        can.setFilterAndMask (0x00000000, 0x00000000, true);
        can.interrupts (true, false);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| RTC                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_RTC
        Rtc rtc;
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| History saved in the flash                                              |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_HISTORY
        History history{rtc};
        FlashEepromStorage<2048, 4> hiScoreStorage (4, 1, size_t (&_hiscore_storage_address));
        hiScoreStorage.init ();
        history.setHiScoreStorage (&hiScoreStorage);
        FlashEepromStorage<2048, 4> historyStorage (12, 2, size_t (&_history_storage_address));
        historyStorage.init ();
        history.setHistoryStorage (&historyStorage);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Backlight, beeper                                                       |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_SOUND
        Gpio buzzerPin (GPIOB, GPIO_PIN_14);
        Buzzer buzzer (buzzerPin);

        if (config.isBuzzerOn ()) {
                buzzer.beep (20, 0, 1);
        }
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| StopWatch, machine and IR                                               |*/
        /*+-------------------------------------------------------------------------+*/

        StopWatch *stopWatch = StopWatch::singleton ();
        FastStateMachine *fStateMachine = FastStateMachine::singleton ();
        fStateMachine->setStopWatch (stopWatch);

        /*--------------------------------------------------------------------------*/

        // IR on means the state is LOW. Beam interruption means transition from LOW to HI i.e. rising.
        Gpio irTriggerInput (IR_PORT, IR_PINS, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL); // TODO GPIO_PULLDOWN

        // External trigger
        Gpio extTriggerInput (EXT_TRIGGER_INPUT_PORT, EXT_TRIGGER_INPUT_PINS, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLDOWN);
        Gpio extTriggerOutput (EXT_TRIGGER_OUTPUT_PORT, EXT_TRIGGER_OUTPUT_PINS, GPIO_MODE_OUTPUT_PP);
        Gpio extTriggerOutEnable (EXT_TRIGGER_OUT_ENABLE_PORT, EXT_TRIGGER_OUT_ENABLE_PINS, GPIO_MODE_OUTPUT_PP);

        // while (true) {

        //         // extTriggerOutEnable = true;
        //         // extTriggerOutput = true;
        //         // HAL_Delay (6);
        //         // extTriggerOutput = false;
        //         // HAL_Delay (6);
        //         // extTriggerOutEnable = false;

        //         extTriggerOutEnable = true;
        //         extTriggerOutput = true;
        //         HAL_Delay (12);
        //         extTriggerOutput = false;
        //         extTriggerOutEnable = false;

        //         HAL_Delay (1000);
        // }

        InfraRedBeamExti beam{irTriggerInput, extTriggerOutput, extTriggerOutEnable};
        irTriggerInput.setOnToggle ([&beam] { beam.onExti (beam.getPinState (), false); });
        extTriggerInput.setOnToggle (
                [&beam, &extTriggerInput] { beam.onExti ((extTriggerInput.get ()) ? (IrBeam::absent) : (IrBeam::present), true); });

        protocol.setBeam (&beam);
        beam.setFastStateMachine (fStateMachine);
        beam.setStopWatch (stopWatch);

        /*--------------------------------------------------------------------------*/

#ifdef WITH_BUTTON
        Gpio buttonPin (GPIOB, GPIO_PIN_15, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL);
        HAL_NVIC_SetPriority (BUTTON_IRQn, BUTTON_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (BUTTON_IRQn);
        Button button (buttonPin);
#endif

#ifdef WITH_CAN
        FastStateMachineProtocolCallback callback{*fStateMachine};
        protocol.setCallback (&callback);
#endif
        fStateMachine->setIr (&beam);
        fStateMachine->setDisplay (&display);

#ifdef WITH_SOUND
        fStateMachine->setBuzzer (&buzzer);
#endif

#ifdef WITH_HISTORY
        fStateMachine->setHistory (&history);
#endif

#ifdef WITH_CAN
        fStateMachine->setCanProtocol (&protocol);
#endif

        HAL_NVIC_SetPriority (IR_IRQn, IR_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (IR_IRQn);

        HAL_NVIC_SetPriority (EXT_TRIGGER_INPUT_IRQn, EXT_TRIGGER_INPUT_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (EXT_TRIGGER_INPUT_IRQn);

        /*+-------------------------------------------------------------------------+*/
        /*| Battery, light sensor, others                                            |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_POWER_MANAGER
        PowerManagement power{display, *fStateMachine};
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Menu                                                                    |*/
        /*+-------------------------------------------------------------------------+*/

        DisplayMenu menu (config, display, *fStateMachine);

#ifdef WITH_HISTORY
        menu.setHistory (&history);
#endif

#ifdef WITH_RTC
        menu.setRtc (&rtc);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| USB                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_USB
        __disable_irq ();

        /* Initialize Device Library */
        USBD_Init (&USBD_Device, &USBD_Desc, 0);

        /* to work within ST's drivers, I've written a special USBD_Composite class that then invokes several classes */
        USBD_RegisterClass (&USBD_Device, &USBD_Composite);

        /* Start Device Process */
        USBD_Start (&USBD_Device);

        usbOnConnected ([] { showGreeting = true; });

        auto refreshAll = [&menu] {
                menu.onEvent (menu::Event::refreshMenu);
                cfg::changed () = true;
        };

        auto cli = cl::cli<String> (
                cl::cmd (String ("result"), [&history] { history.printHistory (); }),
                cl::cmd (String ("resultMs"), [&history] { history.printHistory (ResultDisplayStyle::milisecondOnly); }),
                cl::cmd (String ("last"), [&history] { history.printLast (); }),
                cl::cmd (String ("lastMs"), [&history] { history.printLast (ResultDisplayStyle::milisecondOnly); }),
                cl::cmd (String ("date"),
                         [&rtc] {
                                 auto r = rtc.getDate ();
                                 printDate (r.first, r.second);
                                 usbWrite ("\r\n\r\n");
                         }),

                cl::cmd (String ("iscounting"), [&fStateMachine] { usbWrite ((fStateMachine->isCounting ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("reset"), [&fStateMachine] { fStateMachine->run (Event::Type::reset); }),

                cl::cmd (String ("clear"),
                         [&history] {
                                 history.clearHiScore ();
                                 history.clearResults ();
                         }),
                cl::cmd (String ("factory"),
                         [&] {
                                 getConfigFlashEepromStorage ().clear ();
                                 readConfigFromFlash ();
                                 refreshAll ();
                         }),
                cl::cmd (String ("help"),
                         [] {
                                 usbWrite ("result, resultMs, last, lastMs, date, isCounting, reset, clear, factory, help, battery, getFlip, "
                                           "setFlip, getIr, setIr, getSn, "
                                           "setSn, getRes, setRes, getAuto, setAuto, periph, getBlind, setBlind\r\n\r\n");
                         }),
                cl::cmd (String ("battery"),
                         [&power] {
                                 std::array<char, 11> buf{};
                                 itoa ((unsigned int)(power.getBatteryVoltage ()), buf.data ());
                                 usbWrite (buf.cbegin ());
                                 usbWrite ("mV, ");

                                 itoa ((unsigned int)(power.getBatteryPercent ()), buf.data ());
                                 usbWrite (buf.cbegin ());
                                 usbWrite ("%\r\n\r\n");
                         }),

                cl::cmd (String ("getFlip"), [] { usbWrite ((getConfig ().isFlip ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setFlip"),
                         [&] (String const &arg) {
                                 getConfig ().setFlip (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getIr"), [] { usbWrite ((getConfig ().isIrSensorOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setIr"),
                         [&] (String const &arg) {
                                 getConfig ().setIrSensorOn (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getSn"), [] { usbWrite ((getConfig ().isBuzzerOn ()) ? ("1\r\n\r\n") : ("0\r\n\r\n")); }),
                cl::cmd (String ("setSn"),
                         [&] (String const &arg) {
                                 getConfig ().setBuzzerOn (bool (std::atoi (arg.c_str ())));
                                 refreshAll ();
                         }),

                cl::cmd (String ("getRes"),
                         [] {
                                 switch (getConfig ().getResolution ()) {
                                 case Resolution::ms_10:
                                         usbWrite ("10ms\r\n\r\n");
                                         break;

                                 case Resolution::ms_1:
                                         usbWrite ("1ms\r\n\r\n");
                                         break;

                                 case Resolution::us_100:
                                         usbWrite ("100us\r\n\r\n");
                                         break;

                                 case Resolution::us_10:
                                         usbWrite ("10us\r\n\r\n");
                                         break;

                                 default:
                                         break;
                                 }
                         }),
                cl::cmd (String ("setRes"),
                         [&] (String const &arg) {
                                 if (arg == "10ms") {
                                         getConfig ().setResolution (Resolution::ms_10);
                                 }
                                 else if (arg == "1ms") {
                                         getConfig ().setResolution (Resolution::ms_1);
                                 }
                                 else if (arg == "100us") {
                                         getConfig ().setResolution (Resolution::us_100);
                                 }
                                 else if (arg == "10us") {
                                         getConfig ().setResolution (Resolution::us_10);
                                 }
                                 else {
                                         usbWrite ("Valid options : 10ms, 1ms, 100us, 10us\r\n\r\n");
                                 }

                                 refreshAll ();
                         }),

                cl::cmd (String ("setAuto"),
                         [&] (String const &arg) {
                                 if (arg == "s") {
                                         getConfig ().setAutoDisplayResult (ResultDisplayStyle::secondFraction);
                                 }
                                 else if (arg == "ms") {
                                         getConfig ().setAutoDisplayResult (ResultDisplayStyle::milisecondOnly);
                                 }
                                 else if (arg == "none") {
                                         getConfig ().setAutoDisplayResult (ResultDisplayStyle::none);
                                 }
                         }),
                cl::cmd (String ("getAuto"),
                         [] {
                                 switch (getConfig ().getAutoDisplayResult ()) {
                                 case ResultDisplayStyle::secondFraction:
                                         usbWrite ("s\r\n\r\n");
                                         break;

                                 case ResultDisplayStyle::milisecondOnly:
                                         usbWrite ("ms\r\n\r\n");
                                         break;

                                 case ResultDisplayStyle::none:
                                         usbWrite ("none\r\n\r\n");
                                         break;

                                 default:
                                         break;
                                 }
                         }),

                cl::cmd (String ("periph"),
                         [&protocol] {
                                 protocol.sendInfoRequest ();
                                 HAL_Delay (RESPONSE_WAIT_TIME_MS);
                                 auto &resp = protocol.getInfoRespDataCollection ();

                                 usbWrite ("Connected peripherals :\r\n");
                                 usbWrite ("type uid beam_state\r\n");

                                 for (auto &periph : resp) {
                                         switch (periph.deviceType) {
                                         case DeviceType::receiver:
                                                 usbWrite ("receiver ");
                                                 break;

                                         case DeviceType::ir_sensor:
                                                 usbWrite ("ir_sensor ");
                                                 break;

                                         default:
                                                 usbWrite ("unknown ");
                                                 break;
                                         }

                                         print ((unsigned)periph.uid);
                                         print (" ");

                                         switch (periph.beamState) {
                                         case BeamState::yes:
                                                 usbWrite ("yes");
                                                 break;

                                         case BeamState::no:
                                                 usbWrite ("no");
                                                 break;

                                         case BeamState::blind:
                                                 usbWrite ("blind");
                                                 break;
                                         }

                                         print ("\r\n");
                                 }

                                 print ("\r\n");
                         }),

                cl::cmd (String ("getBlind"),
                         [&] (String const &arg) {
                                 print ((unsigned)getConfig ().getBlindTime ());
                                 usbWrite ("\r\n\r\n");
                         }),
                cl::cmd (String ("setBlind"),
                         [&] (String const &arg) {
                                 int i = std::atoi (arg.c_str ());

                                 if (i < 0 || i > std::numeric_limits<uint16_t>::max () - 1) {
                                         usbWrite ("Correct values are [0, 65534]\r\n\r\n");
                                         return;
                                 }

                                 getConfig ().setBlindTime (i);
                                 refreshAll ();
                         })

        );

        using CliType = decltype (cli);
        cliPointer = &cli;

        usbOnData ([] (const uint8_t *data, size_t len) {
                auto *cli = reinterpret_cast<CliType *> (cliPointer);

                for (size_t i = 0; i < len; ++i) {
                        auto dt = gsl::span{data, len};
                        cli->input (char (dt[i]));
                }
        });

        /* OK, only *now* it is OK for the USB interrupts to fire */
        __enable_irq ();
#endif

        Timer displayTimer;
        Timer menuTimer;

        int refreshRate = 9; // Something different than 10 so the screen is a little bit out of sync. This way the last digit changes.

        // Refresh stopwatch state to reflect the config.
        auto refreshSettings = [&] {
                display.setFlip (config.isFlip ());
                beam.setActive (config.isIrSensorOn ());
#ifdef WITH_SOUND
                buzzer.setActive (config.isBuzzerOn ());
#endif
                // stopWatch->setResolution (config.getResolution ());
                display.setResolution (config.getResolution ());
        };

        refreshSettings ();
        menu.onEvent (menu::Event::timePassed); // Initial state.

        while (true) {
#ifdef WITH_SOUND
                buzzer.run ();
#endif

#ifdef WITH_BUTTON
                button.run ();
#endif

#ifdef WITH_HISTORY
                history.run ();
#endif

#ifdef WITH_POWER_MANAGER
                power.run ();
#endif

                // #ifdef WITH_IR
                beam.run ();
                // #endif

#ifdef WITH_USB
                cli.run ();
#endif
                if (displayTimer.isExpired ()) {
                        fStateMachine->run (Event::Type::timePassed);
                        displayTimer.start (refreshRate);
                }

#ifdef WITH_BUTTON
                if (button.getPressClear ()) {
                        menu.onEvent (menu::Event::shortPress);
#ifdef WITH_SOUND
                        buzzer.beep (20, 0, 1);
#endif
                }

                if (button.getLongPressClear ()) {
                        menu.onEvent (menu::Event::longPress);
#ifdef WITH_SOUND
                        buzzer.beep (20, 20, 2);
#endif
                }
#endif // WITH_BUTTON

                if (cfg::changed ()) {
                        cfg::changed () = false;
                        refreshSettings ();
#ifdef WITH_FLASH
                        getConfigFlashEepromStorage ().store (reinterpret_cast<uint8_t *> (&config), sizeof (config), 0);
#endif
                }

                if (menuTimer.isExpired ()) {
                        menuTimer.start (250);
                        menu.onEvent (menu::Event::timePassed);
                }

                if (showGreeting) {
                        usbWrite ("GP8 stopwatch version: ");
                        usbWrite (VERSION);
                        usbWrite ("\r\n");
                        showGreeting = false;
                }
        }
}

/*****************************************************************************/

#if defined(PLATFORM_REGULAR) || defined(PLATFORM_HUGE)

void SystemClock_Config ()
{
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

        /** Configure LSE Drive Capability
         */
        HAL_PWR_EnableBkUpAccess ();
        __HAL_RCC_LSEDRIVE_CONFIG (RCC_LSEDRIVE_HIGH);
        /** Initializes the CPU, AHB and APB busses clocks
         */

        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14 | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON; // RCC_HSE_BYPASS;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
        RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
        RCC_OscInitStruct.HSI14CalibrationValue = 16;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#if defined(PLATFORM_REGULAR)
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
#elif defined(PLATFORM_HUGE)
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
#endif
        RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }

        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
                Error_Handler ();
        }

        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_RTC;
        PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;

        if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK) {
                Error_Handler ();
        }
}

#elif defined(PLATFORM_MICRO)

void SystemClock_Config ()
{
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

        /** Initializes the CPU, AHB and APB busses clocks.
         */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14 | RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
        RCC_OscInitStruct.HSI14CalibrationValue = 16;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
        RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }
        /** Initializes the CPU, AHB and APB busses clocks.
         */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
                Error_Handler ();
        }
}

#endif

/*****************************************************************************/

namespace __gnu_cxx {
void __verbose_terminate_handler ()
{
        while (true) {
        }
}
} // namespace __gnu_cxx

extern "C" void __cxa_pure_virtual ()
{
        while (true) {
        }
}

// extern "C" void __cxa_guard_acquire ()
//{
//}

// extern "C" void __cxa_guard_release ()
//{
//}

namespace std {
void __throw_bad_function_call ()
{
        while (true) {
        }
}
} // namespace std

#if 0
void *operator new (size_t size) { return nullptr; }

void operator delete (void *p) {}
void operator delete (void *p, unsigned int) {}
#endif

#ifndef WITH_USB
extern "C" void usbWrite (const char * /* str */) {}
#endif

extern "C" void __gxx_personality_v0 () {}
// extern "C" void _init () {}
