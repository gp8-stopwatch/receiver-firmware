/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Adc.h"
#include "AdcChannel.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "stm32f0xx_ll_adc.h"

#define INTERNAL_TEMPSENSOR_V30                                                                                                                 \
        ((int32_t)1430) /* Internal temperature sensor, parameter V30 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_AVGSLOPE                                                                                                            \
        ((int32_t)4300) /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max    \
                           values. */
#define TEMP30_CAL_ADDR                                                                                                                         \
        ((uint16_t *)((uint32_t)0x1FFFF7B8)) /* Internal temperature sensor, parameter TS_CAL1: TS ADC raw data acquired at a temperature of    \
                                                110 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
#define TEMP110_CAL_ADDR                                                                                                                        \
        ((uint16_t *)((uint32_t)0x1FFFF7C2)) /* Internal temperature sensor, parameter TS_CAL2: TS ADC raw data acquired at a temperature of    \
                                                30 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
#define VDDA_TEMP_CAL ((uint32_t)3300)       /* Vdda value with which temperature sensor has been calibrated in production (+-10 mV). */

/* Internal voltage reference */
// #define VREFINT_CAL                                                                                                                             \
//         ((uint16_t *)((uint32_t)0x1FFFF7BA)) /* Internal temperature sensor, parameter VREFINT_CAL: Raw data acquired at a temperature of 30    \
//                                                 DegC (+-5 DegC), VDDA = 3.3 V (+-10 mV). */
// /* This calibration parameter is intended to calculate the actual VDDA from Vrefint ADC measurement. */

#define VDDA_APPLI ((uint32_t)3300)   /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS ((uint32_t)4095) /* Max digital value with a full range of 12 bits */
#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA) (((ADC_DATA)*VDDA_APPLI) / RANGE_12BITS)

/*****************************************************************************/

Adc::Adc () : channelsNum (0)
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

        ADC_ChannelConfTypeDef sConfig;
        sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;

        if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
                Error_Handler ();
        }

        sConfig.Channel = ADC_CHANNEL_VREFINT;
        if (HAL_ADC_ConfigChannel (&hadc, &sConfig) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_ADCEx_Calibration_Start (&hadc) != HAL_OK) {
                Error_Handler ();
        }

        VREFINT_CAL = *VREFINT_CAL_ADDR;
}

/*****************************************************************************/

void Adc::run ()
{
        // TODO Suboptimal, shold use DMA or interrupts.
        for (int i = 0; i < channelsNum + 2; ++i) {

                if (HAL_ADC_Start (&hadc) != HAL_OK) {
                        Error_Handler ();
                }

                if (HAL_ADC_PollForConversion (&hadc, 10) != HAL_OK) {
                        Error_Handler ();
                }

                // Temperature
                if (i == maxChannelsNo) {
                        temperature = HAL_ADC_GetValue (&hadc);
                        debug->print (temperature);
                }
                // Vref
                else if (i == maxChannelsNo + 1) {
                        VREFINT_DATA = HAL_ADC_GetValue (&hadc);
                        debug->print (VREFINT_DATA);
                }
                else if (VREFINT_DATA != 0) {
                        uint32_t ADC_DATAx = HAL_ADC_GetValue (&hadc);
                        float v = (float (TEMPSENSOR_CAL_VREFANALOG) * ADC_DATAx) / float (RANGE_12BITS);
                        v *= float (VREFINT_CAL) / VREFINT_DATA;
                        channels.at (i)->lastValue = int32_t (v + 0.5F);
                        debug->print (i);
                        debug->print (" ");
                        debug->println (channels.at (i)->lastValue);

#if 0
                        Debug *debug = Debug::singleton ();
                        debug->print (ADC_DATAx);
#endif
                }

#if 0
                debug->print (" ");
#endif
        }

#if 0
        Debug::singleton ()->print ("\r\n");
#endif
}

/*****************************************************************************/

void Adc::addChannel (AdcChannel *channel)
{
        channels.at (channelsNum++) = channel;
        channel->init (this);
}
