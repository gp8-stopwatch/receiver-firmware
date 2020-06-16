/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "PowerManagement.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include <stm32f0xx_ll_adc.h>

/****************************************************************************/

// #define TEMP30_CAL_ADDR ((uint16_t *)((uint32_t)0x1FFFF7B8U))
// #define TEMP110_CAL_ADDR ((uint16_t *)((uint32_t)0x1FFFF7C2U))

/****************************************************************************/

PowerManagement::PowerManagement ()
{
        __HAL_RCC_ADC1_CLK_ENABLE ();

        hadc.Instance = ADC1;

        if (HAL_ADC_DeInit (&hadc) != HAL_OK) {
                Error_Handler ();
        }

        hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
        hadc.Init.Resolution = ADC_RESOLUTION_12B;
        hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
        hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc.Init.LowPowerAutoWait = DISABLE;
        hadc.Init.LowPowerAutoPowerOff = DISABLE;
        hadc.Init.ContinuousConvMode = DISABLE;
        hadc.Init.DiscontinuousConvMode = ENABLE;
        hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc.Init.DMAContinuousRequests = DISABLE;
        hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hadc.Init.SamplingTimeCommon = ADC_SAMPLETIME_239CYCLES_5;

        if (HAL_ADC_Init (&hadc) != HAL_OK) {
                Error_Handler ();
        }

        ADC_ChannelConfTypeDef sConfig{};
        // sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;

        // if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
        //         Error_Handler ();
        // }

        sConfig.Channel = ADC_CHANNEL_VREFINT;
        if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_ADCEx_Calibration_Start (&hadc) != HAL_OK) {
                Error_Handler ();
        }

        // Ambient light
        __HAL_RCC_GPIOA_CLK_ENABLE ();
        GPIO_InitTypeDef gpioInitStruct{};
        gpioInitStruct.Pin = GPIO_PIN_4;
        gpioInitStruct.Mode = GPIO_MODE_ANALOG;
        gpioInitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init (GPIOA, &gpioInitStruct);

        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;

        if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
                Error_Handler ();
        }

        // Voltage
        __HAL_RCC_GPIOB_CLK_ENABLE ();
        gpioInitStruct.Pin = GPIO_PIN_0;
        HAL_GPIO_Init (GPIOB, &gpioInitStruct);

        sConfig.Channel = ADC_CHANNEL_8;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;

        if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
                Error_Handler ();
        }

        senseOn = false;
}

/****************************************************************************/

void PowerManagement::run ()
{
        uint32_t VREFINT_CAL = *VREFINT_CAL_ADDR;
        // uint32_t TEMP30_CAL = *TEMP30_CAL_ADDR;
        // uint32_t TEMP110_CAL = *TEMP110_CAL_ADDR;

        senseOn = true;
        HAL_Delay (1);

        // TODO Suboptimal, shold use DMA or interrupts.
        for (int i = 0; i <= int (Channels::vref); ++i) {

                if (HAL_ADC_Start (&hadc) != HAL_OK) {
                        Error_Handler ();
                }

                if (HAL_ADC_PollForConversion (&hadc, 10) != HAL_OK) {
                        Error_Handler ();
                }

                switch (Channels (i)) {
                case Channels::ambient:
                        lastAmbientLight = HAL_ADC_GetValue (&hadc);
                        break;

                case Channels::battery: {
                        if (VREFINT_DATA == 0) {
                                break;
                        }

                        uint32_t ADC_DATAx = HAL_ADC_GetValue (&hadc);
                        float v = (float (TEMPSENSOR_CAL_VREFANALOG) * ADC_DATAx) / float (RANGE_12BITS);
                        v *= float (VREFINT_CAL) / VREFINT_DATA;
                        lastBatteryVoltage = int32_t (v + 0.5F) * 2 + 20; // Experimentally

                        if (lastBatteryVoltage < 3000) {
                                sleep ();
                        }
                } break;

                        // case Channels::temperature: {
                        //         uint32_t rawTemp = HAL_ADC_GetValue (&hadc);
                        //         lastTemperature = ((rawTemp - TEMP30_CAL) * (110U - 30U)) / (TEMP110_CAL - TEMP30_CAL);
                        //         debug->println (rawTemp);
                        // } break;

                case Channels::vref:
                        VREFINT_DATA = HAL_ADC_GetValue (&hadc);
                        break;
                }
        }

        senseOn = false;
}

/****************************************************************************/

void PowerManagement::sleep ()
{
        __HAL_FLASH_PREFETCH_BUFFER_DISABLE ();
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Set all GPIO in analog state to reduce power consumption */
        __HAL_RCC_GPIOA_CLK_ENABLE ();
        __HAL_RCC_GPIOB_CLK_ENABLE ();
        __HAL_RCC_GPIOC_CLK_ENABLE ();
        __HAL_RCC_GPIOD_CLK_ENABLE ();
        __HAL_RCC_GPIOE_CLK_ENABLE ();
        __HAL_RCC_GPIOF_CLK_ENABLE ();

        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        GPIO_InitStructure.Pin = GPIO_PIN_All;

        HAL_GPIO_Init (GPIOA, &GPIO_InitStructure);
        HAL_GPIO_Init (GPIOB, &GPIO_InitStructure);
        HAL_GPIO_Init (GPIOC, &GPIO_InitStructure);
        HAL_GPIO_Init (GPIOD, &GPIO_InitStructure);
        HAL_GPIO_Init (GPIOE, &GPIO_InitStructure);
        HAL_GPIO_Init (GPIOF, &GPIO_InitStructure);

        __HAL_RCC_GPIOA_CLK_DISABLE ();
        __HAL_RCC_GPIOB_CLK_DISABLE ();
        __HAL_RCC_GPIOC_CLK_DISABLE ();
        __HAL_RCC_GPIOD_CLK_DISABLE ();
        __HAL_RCC_GPIOE_CLK_DISABLE ();
        __HAL_RCC_GPIOF_CLK_DISABLE ();

        /* Enable PWR clock */
        __HAL_RCC_PWR_CLK_ENABLE ();

        /*Suspend Tick increment to prevent wakeup by Systick interrupt..
                    Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
        HAL_SuspendTick ();
        HAL_PWR_EnterSTANDBYMode ();
}