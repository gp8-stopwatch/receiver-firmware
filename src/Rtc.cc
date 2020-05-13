/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Rtc.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include <stm32f0xx_hal.h>
// #include <stm32f4xx_hal_rtc_ex.h>

RTC_HandleTypeDef rtcHandle;

/*****************************************************************************/

void RTC_CalendarConfig ()
{
        RTC_DateTypeDef sdatestructure;
        RTC_TimeTypeDef stimestructure;

        /*##-1- Configure the Date #################################################*/
        /* Set Date: Tuesday February 18th 2016 */
        sdatestructure.Year = 0x16;
        sdatestructure.Month = RTC_MONTH_FEBRUARY;
        sdatestructure.Date = 0x18;
        sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

        if (HAL_RTC_SetDate (&rtcHandle, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK) {
                /* Initialization Error */
                Error_Handler ();
        }

        /*##-2- Configure the Time #################################################*/
        /* Set Time: 02:00:00 */
        stimestructure.Hours = 0x02;
        stimestructure.Minutes = 0x00;
        stimestructure.Seconds = 0x00;
        stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
        stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

        if (HAL_RTC_SetTime (&rtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK) {
                /* Initialization Error */
                Error_Handler ();
        }

        /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
        HAL_RTCEx_BKUPWrite (&rtcHandle, RTC_BKP_DR1, 0x32F2);
}

/*****************************************************************************/

Rtc::Rtc ()
{
        __HAL_RCC_PWR_CLK_ENABLE ();
        HAL_PWR_EnableBkUpAccess ();

        // Analogicznie do SystemClock_Config
        RCC_OscInitTypeDef RCC_OscInitStruct;
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }

        // Ta struktura to jest takie dodatkowe DTO i ono służy do konfiguracji RTC i I2S
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

        if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInitStruct) != HAL_OK) {
                Error_Handler ();
        }

        __HAL_RCC_RTC_ENABLE ();

        // HAL_NVIC_SetPriority (RTC_Alarm_IRQn, 0x0F, 0);
        // HAL_NVIC_EnableIRQ (RTC_Alarm_IRQn);
        // HAL_NVIC_SetPriority (RTC_WKUP_IRQn, 0x0F, 0);
        // HAL_NVIC_EnableIRQ (RTC_WKUP_IRQn);

        /*---------------------------------------------------------------------------*/

        rtcHandle.Instance = RTC;
        rtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
        rtcHandle.Init.AsynchPrediv = 127; // 127 to jest max (7bit) i on daje podział na 128!
        rtcHandle.Init.SynchPrediv = 255;  // 15 bit. To są countery i dla tego im większa liczbam, tym wolniejszy sygnał na wyjściu.
        rtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
        rtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        rtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

        if (HAL_RTC_Init (&rtcHandle) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_RTCEx_BKUPRead (&rtcHandle, RTC_BKP_DR1) != 0x32F2) {
                /* Configure RTC Calendar */
                RTC_CalendarConfig ();
        }
        else {
                /* Check if the Power On Reset flag is set */
                if (__HAL_RCC_GET_FLAG (RCC_FLAG_PORRST) != RESET) {
                        debug->println ("Power on reset");
                }
                /* Check if Pin Reset flag is set */
                if (__HAL_RCC_GET_FLAG (RCC_FLAG_PINRST) != RESET) {
                        debug->println ("Pin (external) reset");
                }
        }

        __HAL_RCC_CLEAR_RESET_FLAGS ();
}

/*****************************************************************************/

void Rtc::activateWakeup (uint32_t timeUnits)
{
        /* Disable Wake-up timer */
        if (HAL_RTCEx_DeactivateWakeUpTimer (&rtcHandle) != HAL_OK) {
                Error_Handler ();
        }

        /* Enable Wake-up timer */
        if (HAL_RTCEx_SetWakeUpTimer_IT (&rtcHandle, timeUnits, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

void Rtc::deactivateWakeup ()
{
        if (HAL_RTCEx_DeactivateWakeUpTimer (&rtcHandle) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

void rtcTimeShow ()
{
        RTC_DateTypeDef sdatestructureget;
        RTC_TimeTypeDef stimestructureget;

        /* Get the RTC current Time */
        HAL_RTC_GetTime (&rtcHandle, &stimestructureget, RTC_FORMAT_BIN);
        /* Get the RTC current Date */
        HAL_RTC_GetDate (&rtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
        /* Display time Format : hh:mm:ss */
        //        printf ("%d:%d:%d\n", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
        //        usb.log (SATELITES_NUMBER, SATELITES_NUMBER_T, &stimestructureget.Hours);
        //        usb.log (SATELITES_NUMBER, SATELITES_NUMBER_T, &stimestructureget.Minutes);
        //        usb.log (SATELITES_NUMBER, SATELITES_NUMBER_T, &stimestructureget.Seconds);
}

// void Rtc::setDate (MicroDate d)
// {
//         RTC_TimeTypeDef sTime;
//         RTC_DateTypeDef sDate;
//         microDateToCube (d, &sTime, &sDate);

//         if (HAL_RTC_SetDate (&rtcHandle, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
//                 Error_Handler ();
//         }

//         if (HAL_RTC_SetTime (&rtcHandle, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
//                 Error_Handler ();
//         }
// }

/*****************************************************************************/

void Rtc::getDate () const
{
        RTC_DateTypeDef sDate;
        RTC_TimeTypeDef sTime;

        HAL_RTC_GetTime (&rtcHandle, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate (&rtcHandle, &sDate, RTC_FORMAT_BIN);

        debug->print (sDate.Year);
        debug->print ("-");
        debug->print (sDate.Month);
        debug->print ("-");
        debug->print (sDate.Date);
        debug->print (", ");

        debug->print (sTime.Hours);
        debug->print (":");
        debug->print (sTime.Minutes);
        debug->print (":");
        debug->println (sTime.Seconds);

        // return d;
}

/*****************************************************************************/

void Rtc::backupRegisterWrite (uint32_t backupRegister, uint32_t data) { HAL_RTCEx_BKUPWrite (&rtcHandle, backupRegister, data); }

/*---------------------------------------------------------------------------*/

uint32_t Rtc::backupRegisterRead (uint32_t backupRegister) const { return HAL_RTCEx_BKUPRead (&rtcHandle, backupRegister); }

/*****************************************************************************/

// extern "C" void RTC_WKUP_IRQHandler (void)
// {

//         if (__HAL_RTC_WAKEUPTIMER_GET_IT (&rtcHandle, RTC_IT_WUT)) {
//                 /* Get the status of the Interrupt */
//                 if ((uint32_t) (rtcHandle.Instance->CR & RTC_IT_WUT) != (uint32_t)RESET) {
//                         // debugLog (INFO_C, INFO_C_T, "Wakeup", 0);

//                         /* Clear the WAKEUPTIMER interrupt pending bit */
//                         __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG (&rtcHandle, RTC_FLAG_WUTF);
//                 }
//         }

//         /* Clear the EXTI's line Flag for RTC WakeUpTimer */
//         __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG ();
// }