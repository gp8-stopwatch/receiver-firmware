/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Hal.h"
#include "Timer.h"
#include <cstdint>

static volatile uint32_t sysTickCount;

/*****************************************************************************/

void HAL_IncTick () { sysTickCount++; }

/*****************************************************************************/

void HAL_InitTick ()
{
        /** Configure SysTick to generate Interrupt with 1ms period */
        SysTick_Config (SYSCLK_FREQ / 1000 - 1);
}

/*****************************************************************************/

uint32_t HAL_GetTick () { return sysTickCount; }

/*****************************************************************************/

void Timer::start (uint32_t interval)
{
        startTime = HAL_GetTick ();
        this->intervalMs = interval;
}

/*****************************************************************************/

bool Timer::isExpired () const { return HAL_GetTick () - startTime >= intervalMs; }

/*****************************************************************************/

void Timer::delay (uint32_t delayMs)
{
        uint32_t tickStart = HAL_GetTick ();

        while ((HAL_GetTick () - tickStart) < delayMs) {
        }
}

/*****************************************************************************/

uint32_t Timer::getTick () { return sysTickCount; }
