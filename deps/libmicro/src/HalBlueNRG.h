/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef HAL_BULE_NRG1
#define HAL_BULE_NRG1

#if defined(NRG1)
#include <BlueNRG1.h>
#elif defined(NRG2)
#include <BlueNRG2.h>
#endif
#include <BlueNRG1_adc.h>
#include <BlueNRG1_dma.h>
#include <BlueNRG1_flash.h>
#include <BlueNRG1_gpio.h>
#include <BlueNRG1_i2c.h>
#include <BlueNRG1_mft.h>
#include <BlueNRG1_pka.h>
#include <BlueNRG1_radio.h>
#include <BlueNRG1_rng.h>
#include <BlueNRG1_rtc.h>
#include <BlueNRG1_spi.h>
#include <BlueNRG1_sysCtrl.h>
#include <BlueNRG1_uart.h>
#include <BlueNRG1_wdg.h>
#include <misc.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void HAL_IncTick ();
extern void HAL_InitTick ();
extern uint32_t HAL_GetTick ();

#ifdef __cplusplus
}
#endif

#endif
