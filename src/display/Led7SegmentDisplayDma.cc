/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Led7SegmentDisplayDma.h"
#include "ErrorHandler.h"
#include "StopWatch.h"
#include <array>

Led7SegmentDisplayDma::Led7SegmentDisplayDma ()
{
        /*--------------------------------------------------------------------------*/
        /* Segment timer & DMA. This drives single segments.                        */
        /*--------------------------------------------------------------------------*/

        static TIM_HandleTypeDef htim{};

        htim.Instance = TIM15;
        htim.Init.Prescaler = 48 - 1;
        htim.Init.Period = 200 - 1;
        htim.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim.Init.RepetitionCounter = 0;
        htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        __HAL_RCC_TIM15_CLK_ENABLE ();
        __HAL_RCC_DMA1_CLK_ENABLE ();

        static DMA_HandleTypeDef dmaHandle{};

        dmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        dmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
        dmaHandle.Init.MemInc = DMA_MINC_ENABLE;
        dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        dmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        dmaHandle.Init.Mode = DMA_CIRCULAR;
        dmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
        dmaHandle.Instance = DMA1_Channel5;

        __HAL_LINKDMA (&htim, hdma[TIM_DMA_ID_UPDATE], dmaHandle);

        if (HAL_DMA_Init (htim.hdma[TIM_DMA_ID_UPDATE]) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (displayBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOA->BSRR),
                           displayBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        __HAL_TIM_ENABLE_DMA (&htim, TIM_DMA_UPDATE);

        if (HAL_TIM_Base_Start (&htim) != HAL_OK) {
                Error_Handler ();
        }

        /*--------------------------------------------------------------------------*/
        /* Enable (common pin) timer & DMA. This enables individual displays.       */
        /*--------------------------------------------------------------------------*/

        htim.Instance = TIM7;
        htim.Init.Prescaler = 48 - 1;
        htim.Init.Period = 200 - 1;
        __HAL_RCC_TIM7_CLK_ENABLE ();
        dmaHandle.Instance = DMA1_Channel4;
        __HAL_LINKDMA (&htim, hdma[TIM_DMA_ID_UPDATE], dmaHandle);

        if (HAL_DMA_Init (htim.hdma[TIM_DMA_ID_UPDATE]) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_Base_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (enableBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOB->BSRR),
                           enableBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        __HAL_TIM_ENABLE_DMA (&htim, TIM_DMA_UPDATE);

        if (HAL_TIM_Base_Start (&htim) != HAL_OK) {
                Error_Handler ();
        }

        /*--------------------------------------------------------------------------*/
        /* Synchronization of the above two timers.                                 */
        /*--------------------------------------------------------------------------*/

        TIM15->CNT = 0;
        TIM7->CNT = 0;
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setDigit (uint8_t position, uint8_t digit)
{
        // if (digit >= 0 && digit <= 0xf) {
        // }
        // else if (digit >= 48 && digit <= 57) {
        //         digit -= 48;
        // }
        // else if (digit >= 65 /*A*/ && digit <= 90 /*Z*/) {
        //         digit -= 55;
        // }
        // else if (digit >= 97 /*a*/ && digit <= 122 /*z*/) {
        //         digit -= 87;
        // }
        // else if (digit == '.') {
        //         setDot (position, true);
        // }
        // else if (digit == ' ') {
        //         digit = SPACE_CHAR_INDEX;
        // }

        displayBuffer[position] = ALL_SEGMENTS | FONTS[digit];
}

/****************************************************************************/

void Led7SegmentDisplayDma::setTime (Result10us time, Resolution res)
{
        constexpr std::array<int8_t, 9> FACTORS{10, 10, 10, 10, 10, 10, 6, 10, 10};

        unsigned int cntTmp = time / prescaler;

        if ((prescaler > 1) && time % prescaler >= prescaler / 2) {
                ++cntTmp;
        }

        auto factorIndexCopy = factorIndex;

        for (int i = 5; i >= 0; --i, ++factorIndexCopy) {
                auto factor = FACTORS.at (factorIndexCopy);
                setDigit (i, cntTmp % factor);
                cntTmp /= factor;
        }

        switch (res) {
        case Resolution::ms_10:
                setDots (0b001010);
                break;

        case Resolution::ms_1:
                setDots (0b000101);
                break;

        case Resolution::us_100:
                setDots (0b000010);
                break;

        case Resolution::us_10:
                setDots (0b000001);
                break;

        default:
                break;
        }
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setText (const char *s)
{
        std::string_view txt (s);
        setDots (0);

        uint8_t i = 0;
        for (size_t j = 0; j < txt.size (); ++j) {
                char c = txt.at (j);
                setDigit (i, c); // Sets also dots.

                if (j + 1 < txt.size () && txt.at (j + 1) != '.') {
                        ++i;
                }

                if (i >= DISPLAY_NUM) {
                        break;
                }
        }
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setDot (uint8_t number, bool on)
{
        if (on) {
                dots |= (1 << number);
        }
        else {
                dots &= ~(1 << number);
        }
}

/*****************************************************************************/

void Led7SegmentDisplayDma::clear ()
{
        for (size_t i = 0; i < DISPLAY_NUM; ++i) {
                displayBuffer.at (i) = NO_SEGMENTS;
        }

        dots = 0;
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setResolution (Resolution res)
{
        static constexpr std::array INCREMENTS{1, 10, 100, 1000};
        factorIndex = int (res);
        prescaler = INCREMENTS.at (int (res));
}
