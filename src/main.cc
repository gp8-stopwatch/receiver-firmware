/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Adc.h"
#include "AdcChannel.h"
#include "Button.h"
#include "Buzzer.h"
#include "Can.h"
#include "CanProtocol.h"
#include "Debug.h"
#include "FastStateMachine.h"
#include "Gpio.h"
#include "HardwareTimer.h"
#include "History.h"
#include "InfraRedBeam.h"
#include "Led7SegmentDisplay.h"
#include "StopWatch.h"
#include "Timer.h"
#include "Usart.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include <cstdbool>
#include <cstring>
#include <new>
#include <stm32f0xx_hal.h>
#include <storage/FlashEepromStorage.h>

static void SystemClock_Config (void);

#ifdef WITH_USB
USBD_HandleTypeDef usbdDevice;
#endif

/*****************************************************************************/
/**
 * TODO battery level sensing.
 * TODO loop measurements
 * TODO input in console
 * TODO RTC
 * TODO 2 or 3? contestants
 * TODO Time bigger than 16b in history and everywhere else.
 * TODO demko . Cyfry pokazują się od prawej. Najpierw segmenty 1, potem do 0, potem 8. Wszystkie 6. Potem znów od prawej znikają. Tak było w
 * Fz1, Fz6 i Xj6
 * TODO Wyświetlanie zegara.
 * TODO Kiedy nie ma IR, to wyświetlać same kreski, albo -no ir-
 * TODO buzzer volume or if buzzer at all.
 *
 * CAN:
 * Start
 * Stop + time
 *
 *
 * DONE LED multiplexing driven by hardware timer to prevent frying it in case of program hang.
 */
int main ()
{
        HAL_Init ();
        SystemClock_Config ();

        const uint32_t *MICRO_CONTROLLER_UID = new (reinterpret_cast<void *> (0x1FFFF7AC)) uint32_t;

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

        Led7SegmentDisplay screen (sa, sb, sc, sd, se, sf, sg, sdp, d1, d2, d3, d4, d5, d6);

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
        HAL_NVIC_SetPriority (TIM15_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ (TIM15_IRQn);
        tim15.setOnUpdate ([&screen] { screen.refresh (); });

        /*+-------------------------------------------------------------------------+*/
        /*| Backlight, beeper                                                       |*/
        /*+-------------------------------------------------------------------------+*/

        Gpio buzzerPin (GPIOB, GPIO_PIN_14);
        Buzzer buzzer (buzzerPin);
        buzzer.setActive (false);
        buzzer.beep (20, 0, 1);

        Gpio debugUartGpios (GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_AF1_USART1);
        Usart debugUart (USART1, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();
        ::debug->print ("gp8 stopwatch ready. UID : ");
        ::debug->println (*MICRO_CONTROLLER_UID);

        /*+-------------------------------------------------------------------------+*/
        /*| CAN                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

        Gpio canGpios (GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF4_CAN);

        // 24 - 125kbps
        Can can (nullptr, 24, CAN_SJW_3TQ, CAN_BS1_12TQ, CAN_BS2_3TQ);
        HAL_NVIC_SetPriority (CEC_CAN_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ (CEC_CAN_IRQn);

        CanProtocol protocol (can, *MICRO_CONTROLLER_UID);
        can.setCanCallback (&protocol);
        can.setFilterAndMask (0x00000000, 0x00000000, true);
        can.interrupts (true);

        /*+-------------------------------------------------------------------------+*/
        /*| History saved in the flash                                              |*/
        /*+-------------------------------------------------------------------------+*/

        History *history = History::singleton (/*3*/);
        FlashEepromStorage<2048> hiScoreStorage (2, 1, 0x801E800 /*0x08020000 - 3 * 2048*/);
        hiScoreStorage.init ();
        history->setHiScoreStorage (&hiScoreStorage);
        FlashEepromStorage<2048> historyStorage (2, 2, 0x801F000 /*0x08020000 - 2 * 2048*/);
        historyStorage.init ();
        history->setHistoryStorage (&historyStorage);
        history->init ();
        history->printHistory ();

        /*+-------------------------------------------------------------------------+*/
        /*| StopWatch, machine and IR                                               |*/
        /*+-------------------------------------------------------------------------+*/

        StopWatch *stopWatch = StopWatch::singleton ();
        stopWatch->setDisplay (&screen);
        FastStateMachine *fStateMachine = FastStateMachine::singleton ();
        fStateMachine->setStopWatch (stopWatch);
        stopWatch->setStateMachine (fStateMachine);
        InfraRedBeam beam;
        beam.setActive (false);

        HardwareTimer tim1 (TIM1, 48 - 1, 100 - 1);
        Gpio encoderPins (GPIOA, GPIO_PIN_8, GPIO_MODE_AF_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH, GPIO_AF2_TIM1);
        InputCaptureChannel inputCapture0 (&tim1, 0, true);
        HAL_NVIC_SetPriority (TIM1_BRK_UP_TRG_COM_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ (TIM1_BRK_UP_TRG_COM_IRQn);

        // TODO those are implemented using std::function internally which in turn can cause dynamic allocation
        inputCapture0.setOnIrq ([&beam] { beam.on1kHz (); });
        tim1.setOnUpdate ([&beam] { beam.on10kHz (); });

        /*****************************************************************************/

        //        Button *button = Button::singleton ();
        //        button->init (GPIOB, GPIO_PIN_15);
        fStateMachine->setIr (&beam);
        fStateMachine->setDisplay (&screen);
        fStateMachine->setBuzzer (&buzzer);
        fStateMachine->setHistory (history);
        fStateMachine->setCanProtocol (&protocol);
        //        fStateMachine->setButton (button);

        stopWatch->init ();

        /*+-------------------------------------------------------------------------+*/
        /*| Battery, light sensor                                                   |*/
        /*+-------------------------------------------------------------------------+*/

        Adc *adc = Adc::instance (2);
        adc->init ();

        AdcChannel ambientLightVoltMeter (GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
        adc->addChannel (&ambientLightVoltMeter);

        AdcChannel batteryVoltMeter (GPIOB, GPIO_PIN_0, ADC_CHANNEL_8);
        adc->addChannel (&batteryVoltMeter);
        Timer batteryTimer;

        /*+-------------------------------------------------------------------------+*/
        /*| USB                                                                     |*/
        /*+-------------------------------------------------------------------------+*/

#ifdef WITH_USB

        /* Init Device Library */
        USBD_Init (&usbdDevice, &VCP_Desc, 0);

        /* Add Supported Class */
        USBD_RegisterClass (&usbdDevice, USBD_CDC_CLASS);

        /* Add CDC Interface Class */
        USBD_CDC_RegisterInterface (&usbdDevice, &USBD_CDC_fops);

        /* Start Device Process */
        USBD_Start (&usbdDevice);
#endif

        while (1) {
                buzzer.run ();
                protocol.run ();

                //                if (canTimer.isExpired ()) {
                //                        can.send (CanFrame{ 0x9ABCDEF, true, 1, 0x37 });
                //                        canTimer.start (1000);
                //                }

                //                button->run ();

                if (batteryTimer.isExpired ()) {
                        adc->run ();
                        batteryTimer.start (1000);
                        uint32_t batteryVoltage = batteryVoltMeter.getValue ();
#if 0
                        debug.print ("Battery voltage : ");
                        debug.println (batteryVoltage);
#endif
                        //                        if (batteryVoltage <= 125) {
                        //                                screen->setBatteryLevel (1);
                        //                        }
                        //                        else if (batteryVoltage <= 130) {
                        //                                screen->setBatteryLevel (2);
                        //                        }
                        //                        else if (batteryVoltage <= 140) {
                        //                                screen->setBatteryLevel (3);
                        //                        }
                        //                        else if (batteryVoltage <= 148) {
                        //                                screen->setBatteryLevel (4);
                        //                        }
                        //                        else {
                        //                                screen->setBatteryLevel (5);
                        //                        }

                        uint32_t ambientLightVoltage = ambientLightVoltMeter.getValue ();

                        /*
                         * 50- : 1
                         * 50-100 : 2
                         * 100-150 : 3
                         * 150-200 : 4
                         * 200+ : 5
                         */

                        uint8_t newBrightness = (std::max<uint8_t> ((ambientLightVoltage - 1), 0) / 50) + 1;

#if 0
                        debug.print ("Ambient : ");
                        debug.print (ambientLightVoltage);
                        debug.print (", brightness : ");
                        debug.println (newBrightness);
#endif
                        screen.setBrightness (newBrightness);
                }
        }
}

/*****************************************************************************/

void SystemClock_Config ()
{
        RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
        RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
        RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

        /**Configure LSE Drive Capability
         */
        HAL_PWR_EnableBkUpAccess ();
        __HAL_RCC_LSEDRIVE_CONFIG (RCC_LSEDRIVE_HIGH);
        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14 | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
        RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
        RCC_OscInitStruct.HSI14CalibrationValue = 16;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
        RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }
        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
                Error_Handler ();
        }

        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_RTC;
        PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
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
        while (true)
                ;
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
