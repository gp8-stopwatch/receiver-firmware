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
#include "FastStateMachine.h"
#include "Gpio.h"
#include "HardwareTimer.h"
#include "History.h"
#include "InfraRedBeamModulated.h"
#include "Led7SegmentDisplay.h"
#include "PowerManagement.h"
#include "Rtc.h"
#include "StopWatch.h"
#include "Timer.h"
#include "Usart.h"
#include "usbd_cdc.h"
#include "usbd_composite.h"
#include "usbd_desc.h"
#include <cstdbool>
#include <cstring>
#include <etl/cstring.h>
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
};

template <> void output<String> (String const &tok) { usbWrite (tok.c_str ()); }
template <> void output<char> (char const &tok) { usbWriteData (reinterpret_cast<uint8_t const *> (&tok), 1); }
template <> void output<const char *> (const char *const &tok) { usbWrite (tok); }

} // namespace cl

// Hack to be able to pass the cli object pointer to the C-like function.
static void *cliPointer{};

void readConfigFromFlash () { getConfig () = *reinterpret_cast<cfg::Config const *> (getConfigFlashEepromStorage ().read (nullptr, 2, 0, 0)); }

/*****************************************************************************/

int main ()
{
        HAL_Init ();
        SystemClock_Config ();

        /*+-------------------------------------------------------------------------+*/
        /*| Screen                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

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

        Led7SegmentDisplay display (sa, sb, sc, sd, se, sf, sg, sdp, d1, d2, d3, d4, d5, d6);

#if 0
        screen.setDigit (0, 0xa);
        screen.setDigit (1, 0xb);
        screen.setDigit (2, 0xc);
        screen.setDigit (3, 0xd);
        screen.setDigit (4, 0xe);
        screen.setDigit (5, 0xf);

        while (true) {
        }
#endif

        HardwareTimer tim15 (TIM15, 48 - 1, 200 - 1); // Update 5kHz
        HAL_NVIC_SetPriority (TIM15_IRQn, DISPLAY_TIMER_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (TIM15_IRQn);

#ifdef WITH_DISPLAY
        tim15.setOnUpdate ([&display] { display.refresh (); });
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Config                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        const uint32_t *MICRO_CONTROLLER_UID = new (reinterpret_cast<void *> (0x1FFFF7AC)) uint32_t;
        cfg::Config &config = getConfig ();
        readConfigFromFlash ();

        Gpio debugUartGpios (GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_AF1_USART1);
        Usart debugUart (USART1, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();
        // ::debug->print ("gp8 stopwatch ready. UID : ");
        // ::debug->println (*MICRO_CONTROLLER_UID);

        /*+-------------------------------------------------------------------------+*/
        /*| CAN                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_CAN
        Gpio canGpios (GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF4_CAN);

        // 24 - 125kbps
        Can can (nullptr, 24, CAN_SJW_3TQ, CAN_BS1_12TQ, CAN_BS2_3TQ);
        can.setAutomaticRetransmission (false);
        HAL_NVIC_SetPriority (CEC_CAN_IRQn, CAN_BUS_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (CEC_CAN_IRQn);

        CanProtocol protocol (can, *MICRO_CONTROLLER_UID);
        can.setCanCallback (&protocol);
        can.setFilterAndMask (0x00000000, 0x00000000, true);
        can.interrupts (true, false);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| History saved in the flash                                              |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_FLASH
        History history{};
        FlashEepromStorage<2048, 4> hiScoreStorage (4, 1, 0x801E800 /*0x08020000 - 3 * 2048*/);
        hiScoreStorage.init ();
        history.setHiScoreStorage (&hiScoreStorage);
        FlashEepromStorage<2048, 4> historyStorage (4, 2, 0x801F000 /*0x08020000 - 2 * 2048*/);
        historyStorage.init ();
        history.setHistoryStorage (&historyStorage);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Backlight, beeper                                                       |*/
        /*+-------------------------------------------------------------------------+*/

        Gpio buzzerPin (GPIOB, GPIO_PIN_14);
        Buzzer buzzer (buzzerPin);

        if (config.buzzerOn) {
                buzzer.beep (20, 0, 1);
        }

        /*+-------------------------------------------------------------------------+*/
        /*| StopWatch, machine and IR                                               |*/
        /*+-------------------------------------------------------------------------+*/

        StopWatch *stopWatch = StopWatch::singleton ();
        FastStateMachine *fStateMachine = FastStateMachine::singleton ();
        fStateMachine->setStopWatch (stopWatch);

        /*--------------------------------------------------------------------------*/

        Gpio irTriggerPin (GPIOA, GPIO_PIN_8, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL);
        InfraRedBeamExti beam{(irTriggerPin.get ()) ? (IrBeam::absent) : (IrBeam::present)};
        irTriggerPin.setOnToggle ([&beam, &irTriggerPin] { beam.onExti ((irTriggerPin.get ()) ? (IrBeam::absent) : (IrBeam::present)); });
        beam.onTrigger = [fStateMachine] {
#ifdef TEST_TRIGGER_MOD_2
                static int i{};
                if (++i % 2 == 0) {
                        fStateMachine->run (Event::irTrigger);
                }
#else
                fStateMachine->run (Event::irTrigger);
#endif
        };

        /*--------------------------------------------------------------------------*/

        Gpio buttonPin (GPIOB, GPIO_PIN_15, GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL);
        HAL_NVIC_SetPriority (EXTI4_15_IRQn, BUTTON_AND_IR_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (EXTI4_15_IRQn);
        Button button (buttonPin);

        // Test trigger
        Gpio testTriggerPin (GPIOB, GPIO_PIN_3, GPIO_MODE_IT_RISING, GPIO_PULLDOWN);
        HAL_NVIC_SetPriority (EXTI2_3_IRQn, TEST_TRIGGER_EXTI_PRIORITY, 0);
        HAL_NVIC_EnableIRQ (EXTI2_3_IRQn);
        testTriggerPin.setOnToggle ([fStateMachine] {
#ifdef TEST_TRIGGER_MOD_2
                static int i{};
                if (++i % 2 == 0) {
                        fStateMachine->run (Event::testTrigger);
                }
#else
                fStateMachine->run (Event::testTrigger);
#endif
        });

#ifdef WITH_CAN
        protocol.setOnStart ([fStateMachine] { fStateMachine->run (Event::canBusStart); });
        protocol.setOnLoopStart ([fStateMachine] { fStateMachine->run (Event::canBusLoopStart); });
        protocol.setOnStop ([fStateMachine] { fStateMachine->run (Event::canBusStop); });
#endif
        fStateMachine->setIr (&beam);
        fStateMachine->setDisplay (&display);
        fStateMachine->setBuzzer (&buzzer);
#ifdef WITH_FLASH
        fStateMachine->setHistory (&history);
#endif
#ifdef WITH_CAN
        fStateMachine->setCanProtocol (&protocol);
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Battery, light sensor, others                                            |*/
        /*+-------------------------------------------------------------------------+*/

        PowerManagement power;

        /*+-------------------------------------------------------------------------+*/
        /*| RTC                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        Rtc rtc;

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

        usbOnConnected ([] {
                usbWrite ("GP8 stopwatch version: ");
                usbWrite (VERSION);
                usbWrite ("\r\n");
        });

        auto c = cl::cli<String> (cl::cmd (String ("result"), [&history] { history.printHistory (); }),
                                  cl::cmd (String ("last"), [&history] { history.printLast (); }),
                                  cl::cmd (String ("date"), [&rtc] { rtc.getDate (); }),

                                  cl::cmd (String ("store128"),
                                           [&history] {
                                                   for (int i = 0; i < 128; ++i) {
                                                           history.store (i);
                                                   }
                                           }),
                                  cl::cmd (String ("store127"),
                                           [&history] {
                                                   for (int i = 0; i < 127; ++i) {
                                                           history.store (i);
                                                   }
                                           }),
                                  cl::cmd (String ("store16"),
                                           [&history] {
                                                   for (int i = 0; i < 16; ++i) {
                                                           history.store (i);
                                                   }
                                           }),
                                  cl::cmd (String ("store1"), [&] { history.store (666); }),

                                  cl::cmd (String ("iscounting"),
                                           [&fStateMachine] {
                                                   if (fStateMachine->isCounting ()) {
                                                           usbWrite ("1\r\n\r\n");
                                                   }
                                                   else {
                                                           usbWrite ("0\r\n\r\n");
                                                   }
                                           }),
                                  cl::cmd (String ("reset"), [&fStateMachine] { fStateMachine->run (Event::reset); }),

                                  cl::cmd (String ("clear"),
                                           [&history] {
                                                   history.clearHiScore ();
                                                   history.clearResults ();
                                           }),
                                  cl::cmd (String ("factory"), [] { getConfigFlashEepromStorage ().clear (); }),
                                  cl::cmd (String ("help"), [] { usbWrite ("battery, clear, last, result, reset\r\n\r\n"); }),
                                  cl::cmd (String ("battery"),
                                           [&power] {
                                                   std::array<char, 11> buf{};
                                                   itoa ((unsigned int)(power.getBatteryVoltage ()), buf.data ());
                                                   usbWrite (buf.cbegin ());
                                                   usbWrite ("mV, ");

                                                   itoa ((unsigned int)(power.getBatteryPercent ()), buf.data ());
                                                   usbWrite (buf.cbegin ());
                                                   usbWrite ("%\r\n\r\n");
                                           })

        );

        using CliType = decltype (c);
        cliPointer = &c;

        usbOnData ([] (const uint8_t *data, size_t len) {
                auto *c = reinterpret_cast<CliType *> (cliPointer);

                for (size_t i = 0; i < len; ++i) {
                        auto dt = gsl::span{data, len};
                        c->run (char (dt[i]));
                }
        });

        /* OK, only *now* it is OK for the USB interrupts to fire */
        __enable_irq ();
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Menu                                                                    |*/
        /*+-------------------------------------------------------------------------+*/

        DisplayMenu menu (config, display, *fStateMachine);

        Timer displayTimer;
        Timer batteryTimer;
        int refreshRate = 9; // Something different than 10 so the screen is a little bit out of sync. This way the last digit changes.

        // Refresh stopwatch state to reflect the config.
        auto refresh = [&] {
                display.setFlip (config.orientationFlip);
                beam.setActive (config.irSensorOn);
                buzzer.setActive (config.buzzerOn);
                stopWatch->setResolution (config.resolution);
                display.setResolution (config.resolution);
        };

        refresh ();

        while (true) {
                buzzer.run ();
                button.run ();
                history.run ();

                if (displayTimer.isExpired ()) {
                        fStateMachine->run (Event::timePassed);
                        displayTimer.start (refreshRate);
                }

                if (button.getPressClear ()) {
                        menu.onShortPress ();
                        buzzer.beep (20, 0, 1);
                }

                if (button.getLongPressClear ()) {
                        menu.onLongPress ();
                        buzzer.beep (20, 20, 2);
                }

                if (cfg::changed ()) {
                        cfg::changed () = false;
                        refresh ();
                        getConfigFlashEepromStorage ().store (reinterpret_cast<uint8_t *> (&config), sizeof (config), 0);
                }

                if (batteryTimer.isExpired ()) {
                        power.run ();
                        batteryTimer.start (1000);
                        uint32_t ambientLightVoltage = power.getAmbientLight ();

                        /*
                         * 50- : 1
                         * 50-100 : 2
                         * 100-150 : 3
                         * 150-200 : 4
                         * 200+ : 5
                         */

                        uint8_t newBrightness = (std::max<int> ((int (ambientLightVoltage) - 1), 0) / 819) + 1;

#if 0
                        debug.print ("Ambient : ");
                        debug.print (ambientLightVoltage);
                        debug.print (", brightness : ");
                        debug.println (newBrightness);
#endif
                        display.setBrightness (newBrightness);
                }
        }
}

/*****************************************************************************/

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
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
        RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }
        /** Initializes the CPU, AHB and APB busses clocks
         */
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

// extern "C" void _init () {}
