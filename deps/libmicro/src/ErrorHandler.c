/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#ifdef UNIT_TEST
#include <assert.h>
#endif

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler (void)
{
#ifndef UNIT_TEST
        while (1) {
        }
#else
        assert (0);
#endif
}
