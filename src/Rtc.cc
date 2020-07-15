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

std::pair<RTC_DateTypeDef, Time> Rtc::getDate () const
{
        RTC_DateTypeDef sDate;
        RTC_TimeTypeDef sTime;

        HAL_RTC_GetTime (&rtcHandle, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate (&rtcHandle, &sDate, RTC_FORMAT_BIN);

        Time t{sTime.Hours, sTime.Minutes, sTime.Seconds, sTime.TimeFormat};
        return {sDate, t};
}

/*****************************************************************************/

void Rtc::backupRegisterWrite (uint32_t backupRegister, uint32_t data) { HAL_RTCEx_BKUPWrite (&rtcHandle, backupRegister, data); }

/****************************************************************************/

uint32_t Rtc::backupRegisterRead (uint32_t backupRegister) const { return HAL_RTCEx_BKUPRead (&rtcHandle, backupRegister); }

/****************************************************************************/

void Rtc::timeAdd (Set set)
{
        RTC_TimeTypeDef sTime;
        HAL_RTC_GetTime (&rtcHandle, &sTime, RTC_FORMAT_BIN);

        if (set == Set::hour) {
                ++sTime.Hours;
                sTime.Hours %= 24;
        }

        if (set == Set::minute) {
                ++sTime.Minutes;
                sTime.Minutes %= 60;
        }

        if (set == Set::second) {
                ++sTime.Seconds;
                sTime.Seconds %= 60;
        }

        sTime.TimeFormat = RTC_HOURFORMAT_24;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;

        if (HAL_RTC_SetTime (&rtcHandle, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
                Error_Handler ();
        }
}

/****************************************************************************/

void Rtc::dateAdd (Set set)
{
        RTC_DateTypeDef sDate;
        HAL_RTC_GetDate (&rtcHandle, &sDate, RTC_FORMAT_BIN);

        if (set == Set::year) {
                ++sDate.Year;

                if (sDate.Year < 20 || sDate.Year > 50) {
                        sDate.Year = 20;
                }
        }

        if (set == Set::month) {
                ++sDate.Month;
                sDate.Month %= 12;
        }

        if (set == Set::day) {
                ++sDate.Date;
                sDate.Date %= 31;
        }

        if (HAL_RTC_SetDate (&rtcHandle, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
                Error_Handler ();
        }
}