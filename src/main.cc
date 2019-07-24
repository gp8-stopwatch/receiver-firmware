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
#include "Debug.h"
#include "FastStateMachine.h"
#include "Gpio.h"
#include "History.h"
#include "I2CLcdDataLink.h"
#include "InfraRedBeam.h"
#include "Led7SegmentDisplay.h"
#include "PCF85176Driver.h"
#include "StopWatch.h"
#include "T145003.h"
#include "Timer.h"
#include "Usart.h"
#include "config.h"
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

#if 1
int main ()
{
        HAL_Init ();
        SystemClock_Config ();

        /*+-------------------------------------------------------------------------+*/
        /*| Screen                                                                  |*/
        /*+-------------------------------------------------------------------------+*/

        // I2CLcdDataLink *link = I2CLcdDataLink::singleton ();
        // link->init ();
        //
        // PCF85176Driver *lcdd = PCF85176Driver::singleton ();
        // lcdd->setDataLink (link);
        // lcdd->init (3, 4, true);

        Gpio d1 (GPIOB, GPIO_PIN_11/*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);
        Gpio d2 (GPIOB, GPIO_PIN_12/*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);
        Gpio d3 (GPIOB, GPIO_PIN_13/*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);
        Gpio d4 (GPIOB, GPIO_PIN_10/*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);
        Gpio d5 (GPIOB, GPIO_PIN_2 /*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);
        Gpio d6 (GPIOA, GPIO_PIN_5 /*, GPIO_MODE_AF_OD, GPIO_PULLUP*/);

        Gpio sa (GPIOA, GPIO_PIN_0);
        Gpio sb (GPIOA, GPIO_PIN_1);
        Gpio sc (GPIOA, GPIO_PIN_2);
        Gpio sd (GPIOA, GPIO_PIN_3);
        Gpio se (GPIOA, GPIO_PIN_6);
        Gpio sf (GPIOA, GPIO_PIN_7);
        Gpio sg (GPIOB, GPIO_PIN_0);
        Gpio sdp (GPIOB, GPIO_PIN_1);

//        // True = hi-z, false = 0
//        d1 = d2 = d3 = d4 = d5 = d6 = false;
//        //d6 = false;
//        sa = sb = sc = sd = se = sf = sg = sdp = true;

//        while (true) {
//        }

        Led7SegmentDisplay screen (sa, sb, sc, sd, se, sf, sg, sdp, d1, d2, d3, d4, d5, d6);
        // screen.setDigit (0, 0xa);
        // screen.setDigit (1, 0xb);
        // screen.setDigit (2, 0xc);
        // screen.setDigit (3, 0xd);
        // screen.setDigit (4, 0xe);
        // screen.setDigit (5, 0xf);

        /*+-------------------------------------------------------------------------+*/
        /*| Backlight, beeper                                                       |*/
        /*+-------------------------------------------------------------------------+*/

        Buzzer *buzzer = Buzzer::singleton ();
        buzzer->init ();

#if 0
        Gpio debugUartGpios (GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_AF1_USART1);
        Usart debugUart (USART1, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();
        ::debug->print ("gp8 stopwatch ready\n");
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| StopWatch, machine and IR                                               |*/
        /*+-------------------------------------------------------------------------+*/

        // History *history = History::singleton (/*3*/);
        // FlashEepromStorage<2048> hiScoreStorage (2, 1, 0x801E800 /*0x08020000 - 3 * 2048*/);
        // hiScoreStorage.init ();
        // history->setHiScoreStorage (&hiScoreStorage);
        // FlashEepromStorage<2048> historyStorage (2, 2, 0x801F000 /*0x08020000 - 2 * 2048*/);
        // historyStorage.init ();
        // history->setHistoryStorage (&historyStorage);
        // history->init ();
        // history->printHistory ();

#if 0
        StopWatch *stopWatch = StopWatch::singleton ();
        stopWatch->setDisplay (&screen);
        FastStateMachine *fStateMachine = FastStateMachine::singleton ();
        fStateMachine->setStopWatch (stopWatch);
        stopWatch->setStateMachine (fStateMachine);
        InfraRedBeam *beam = InfraRedBeam::singleton ();

        //        Button *button = Button::singleton ();
        //        button->init (GPIOB, GPIO_PIN_8);
        fStateMachine->setIr (beam);
        fStateMachine->setDisplay (&screen);
        fStateMachine->setBuzzer (buzzer);
        // fStateMachine->setHistory (history);
        //        fStateMachine->setButton (button);

        beam->init ();
        stopWatch->init ();
#endif

        /*+-------------------------------------------------------------------------+*/
        /*| Battery, light sensor                                                   |*/
        /*+-------------------------------------------------------------------------+*/

        // Adc *adc = Adc::instance (2);
        // adc->init ();
        //
        // AdcChannel ambientLightVoltMeter (GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);
        // adc->addChannel (&ambientLightVoltMeter);
        //
        // AdcChannel batteryVoltMeter (GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
        // adc->addChannel (&batteryVoltMeter);
        // Timer batteryTimer;

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
                screen.refresh ();
                buzzer->run ();
                //                button->run ();

                //                if (batteryTimer.isExpired ()) {
                //                        adc->run ();
                //                        batteryTimer.start (1000);
                //                        uint8_t batteryVoltage = batteryVoltMeter.getValue ();

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

                //                        uint8_t ambientLightVoltage = ambientLightVoltMeter.getValue ();

                //                        if (!screen->getBacklight () && ambientLightVoltage < 50) {
                //                                screen->setBacklight (true);
                //                        }
                //                        else if (screen->getBacklight () && ambientLightVoltage > 80) {
                //                                screen->setBacklight (false);
                //                        }
                //                }
        }
}
#endif

/*****************************************************************************/

void SystemClock_Config (void)
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

void *operator new (size_t size) { return nullptr; }

void operator delete (void *p) {}
void operator delete (void *p, unsigned int) {}

// extern "C" void _init () {}
