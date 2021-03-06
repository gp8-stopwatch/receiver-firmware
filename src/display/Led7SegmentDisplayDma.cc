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

// Led7SegmentDisplayDma *instance{};

/****************************************************************************/

uint16_t flipFont (uint16_t font)
{
        return (font & 0x06) << 5 | (font & 0xc0) >> 5 | (font & 0x20) | (font & 0x08) >> 3 | (font & 0x01) << 3 | (font & 0x100);
}

/****************************************************************************/

// Led7SegmentDisplayDma *instance{};

Led7SegmentDisplayDma::Led7SegmentDisplayDma ()
{ /* init (DEFAULT_FPS); */
}

void Led7SegmentDisplayDma::init (uint16_t fps)
{
        // instance = this;

        // {
        //         /**TIM15 GPIO Configuration
        //         PB14     ------> TIM15_CH1
        //         */
        //         __HAL_RCC_GPIOB_CLK_ENABLE ();
        //         GPIO_InitTypeDef GPIO_InitStruct{};
        //         GPIO_InitStruct.Pin = GPIO_PIN_14;
        //         GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        //         GPIO_InitStruct.Pull = GPIO_NOPULL;
        //         GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        //         GPIO_InitStruct.Alternate = GPIO_AF1_TIM15;
        //         HAL_GPIO_Init (GPIOB, &GPIO_InitStruct);
        // }

        // {
        //         __HAL_RCC_GPIOA_CLK_ENABLE ();
        //         /**TIM1 GPIO Configuration
        //         PA8     ------> TIM1_CH1
        //         */
        //         GPIO_InitTypeDef GPIO_InitStruct{};
        //         GPIO_InitStruct.Pin = GPIO_PIN_8;
        //         GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        //         GPIO_InitStruct.Pull = GPIO_NOPULL;
        //         GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        //         GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
        //         HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);
        // }

        // DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM1_STOP;
        // DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM15_STOP;

        TIM1->CR1 &= ~TIM_CR1_CEN;
        TIM1->DIER = 0;
        // TIM15->CR1 &= ~TIM_CR1_CEN;

        /*
         * Only counter overflow generates an update event. If it wer set to 0,
         * then also setting UG bit and update generation through slave mode
         * controller would generarte this.
         */
        // TIM1->CR1 |= TIM_CR1_URS;
        // TIM15->CR1 |= TIM_CR1_URS;

        // ARR register is not buffered (0)
        // TIM1->CR1 &= TIM_CR1_ARPE;
        // TIM16->CR1 &= TIM_CR1_ARPE;

        /*
         * Brightness is physically changed by changing the duty cycle of TIM1 channel1.
         * TODO For smoe reason the scren initializes properly only with brigthness set to max
         */
        /* prevBrightness = */ brightness = MAX_BRIGHTNESS;
        recalculateBrightnessTable (fps);

        /*--------------------------------------------------------------------------*/
        /* Enable (common pin) timer & DMA. This enables individual displays.       */
        /*--------------------------------------------------------------------------*/

        TIM_HandleTypeDef htimEn{};

        htimEn.Instance = TIM1;
        htimEn.Init.Prescaler = PRESCALER - 1;
        htimEn.Init.Period = calculatePeriod (fps) /* - 1 */;     // Period is constant and equals 50 (not 49). Read below.
        htimEn.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED3; // This is very important, read next comment below.
        htimEn.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htimEn.Init.RepetitionCounter = 1;
        htimEn.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

        HAL_TIM_PWM_DeInit (&htimEn);

        /*
         * In the above piece of code I configured the TIM1 in the so called "center aligned"
         * mode. If the calculatePeriod (fps) retured 50, and we would substract 1 as usual,
         * the counter register would follow this progression: 0, 1, 2 ... 47, 48, 49, 48, 47 ... 2, 1,  |  0, 1, 2 ...
         *
         * I marked the end of the cycle with pipe character "|"" which helps to realize, that the
         * true number of counter incerements in ecah update cycle would not equal 100 but rather 98!
         * Notice that after 49 (i.e. after 50th tick) the counter register counts from 48 to 1, so we
         * have 50 + 48 ticks.
         *
         * And this is why the "-1" is commented out in the above code, but is left in the TIM15
         * configuration.
         */

        __HAL_RCC_TIM1_CLK_ENABLE ();
        __HAL_RCC_DMA1_CLK_ENABLE ();

        DMA_HandleTypeDef dmaHandle{};

        dmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        dmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
        dmaHandle.Init.MemInc = DMA_MINC_ENABLE;
        dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        dmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        dmaHandle.Init.Mode = DMA_CIRCULAR;
        dmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
        dmaHandle.Instance = DMA1_Channel2;

        // HAL_DMA_DeInit (&dmaHandle);

        // DMA1_Channel2->CCR |= /* DMA_CCR_TCIE */ DMA_CCR_TEIE; // Transfer complete enable

        __HAL_LINKDMA (&htimEn, hdma[TIM_DMA_ID_CC1], dmaHandle);

        if (HAL_DMA_Init (htimEn.hdma[TIM_DMA_ID_CC1]) != HAL_OK) {
                Error_Handler ();
        }

        // HAL_Delay (1);

        if (HAL_TIM_PWM_Init (&htimEn) != HAL_OK) {
                Error_Handler ();
        }

        // TIM1->SR = 0;
        // // __HAL_TIM_DISABLE_DMA (&htimEn, TIM_DMA_CC1);
        // // __HAL_TIM_ENABLE_DMA (&htimEn, TIM_DMA_CC1);
        // TIM1->DIER = 0;
        // TIM1->DIER |= TIM_DIER_CC1DE;

        /*
         * enableBuffer (12 elements, each 32 bit) is transferred constatntly (thanks
         * to the DMA circular mode) to the GPIOB->BSRR. First 16 bits of every element
         * of this buffer is for clearing, and less signifficat 16 bits are for setting.
         */
        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (enableBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOB->BSRR),
                           enableBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        // HAL_Delay (1);

        /*--------------------------------------------------------------------------*/

        /*
         * TIM1 is configured as a slave (this is the only option to synchronize TIM1 and
         * TIM15. I need TIM1 as this is the only remaining timer which implements the
         * center aligned mode.
         */
        // TIM_SlaveConfigTypeDef sSlaveConfig{};
        // sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
        // sSlaveConfig.InputTrigger = TIM_TS_ITR0; // TIM15 is the master. See table 62 or 71.
        // sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
        // sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;

        // if (HAL_TIM_SlaveConfigSynchro (&htimEn, &sSlaveConfig) != HAL_OK) {
        //         Error_Handler ();
        // }

        /*--------------------------------------------------------------------------*/

        /*
         * And this is how the brightness is implemented. The TIM1 is set up in the
         * center aligned mode (lets assume ARR == 50). Both timers would follow
         * this plot:
         *
         *              -            -            -            -
         *             / \          / \          / \          / \
         *            /   \        /   \        /   \        /   \
         *  TIM1     /     \      /     \      /     \      /     \
         *          a       b    /       \    /       \    /       \
         *         /         \  /         \  /         \  /         \
         *        /           \/           \/           \/           \
         *
         *                    c            |            -            -
         *                  -/|          -/|          -/|          -/|
         *                -/  |        -/  |        -/  |        -/  |
         *  TIM15       -/    |      -/    |      -/    |      -/    |
         *            -/      |    -/      |    -/      |    -/      |
         *          -/        |  -/        |  -/        |  -/        |
         *        -/          |-/          |-/          |-/          |
         *
         * As time passes, at the point "a" the apropriate enable pin gets turned
         * ON by the DMA, and then it gets turned off at the point "b". DMA is fired
         * when the counter (CNT) value matches that configured  in the output compare channel 1
         * (see directly below). This match occurs 2 times every TIM15 perid. At the point
         * "c" next character is transferred byt he DMA to the "segment" pins. Changing
         * the distance between points "a" and "b" the brightness is altered.
         */

        TIM_OC_InitTypeDef sConfig{};
        sConfig.OCMode = TIM_OCMODE_PWM1;
        sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfig.OCFastMode = TIM_OCFAST_DISABLE;
        sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
        sConfig.Pulse = brightnessLookup.at (brightness - 1); // <- this controls the brightness. Low Pulse value means high intensity.
        // sConfig.Pulse = 3; // <- this controls the brightness. Low Pulse value means high intensity.

        if (HAL_TIM_PWM_ConfigChannel (&htimEn, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
                Error_Handler ();
        }

        /*--------------------------------------------------------------------------*/

        dmaHandle.Init.Priority = DMA_PRIORITY_LOW; // Different priority than the other channel
        dmaHandle.Instance = DMA1_Channel5;

        // HAL_DMA_DeInit (&dmaHandle);

        __HAL_LINKDMA (&htimEn, hdma[TIM_DMA_ID_UPDATE], dmaHandle);

        if (HAL_DMA_Init (htimEn.hdma[TIM_DMA_ID_UPDATE]) != HAL_OK) {
                Error_Handler ();
        }

        // HAL_Delay (1);

        // TIM1->SR = 0;
        // // __HAL_TIM_DISABLE_DMA (&htimEn, TIM_DMA_UPDATE);
        // // __HAL_TIM_ENABLE_DMA (&htimEn, TIM_DMA_UPDATE);

        // TIM1->DIER = 0;
        // TIM1->DIER |= TIM_DIER_CC1DE | TIM_DIER_UDE;

        // DMA1_Channel5->CCR |= DMA_CCR_TCIE;

        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (displayBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOA->BSRR),
                           displayBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        TIM1->DIER |= TIM_DIER_CC1DE | TIM_DIER_UDE;

        // HAL_Delay (1);

        /*--------------------------------------------------------------------------*/
        /* Segment timer & DMA. This drives single segments.                        */
        /*--------------------------------------------------------------------------*/

        // TIM_HandleTypeDef htimSeg{};
        // htimSeg.Instance = TIM15;
        // htimSeg.Init.Prescaler = PRESCALER - 1;
        // htimSeg.Init.Period = calculatePeriod (fps) * 2 - 1; // Counter counts from 0 to 99, meaning 100 ticks.
        // htimSeg.Init.CounterMode = TIM_COUNTERMODE_UP;
        // htimSeg.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        // htimSeg.Init.RepetitionCounter = 0;
        // htimSeg.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

        // HAL_TIM_PWM_DeInit (&htimSeg);

        /*
         * Above, the period is set to twice the value compared to TIM1 minus 1 but the
         * mode is UP instead of the "center-aligned". If calculatePeriod (fps) gave 50,
         * then the counter would conunt from 0 to 99 which is exactly 100 times. As You
         * can see this is two times more than what TIM1 counts to.
         */

        // __HAL_RCC_TIM15_CLK_ENABLE ();

        /*--------------------------------------------------------------------------*/

        // TIM_MasterConfigTypeDef sMasterConfig{};
        // sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
        // sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;

        // if (HAL_TIMEx_MasterConfigSynchronization (&htimSeg, &sMasterConfig) != HAL_OK) {
        //         Error_Handler ();
        // }

        // // sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
        // // sSlaveConfig.InputTrigger = TIM_TS_TI1F_ED; // As described on page 433 in the RM.
        // // sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
        // // sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;

        // // if (HAL_TIM_SlaveConfigSynchro (&htimSeg, &sSlaveConfig) != HAL_OK) {
        // //         Error_Handler ();
        // // }

        // TIM15->SMCR |= TIM_SMCR_MSM;

        /*--------------------------------------------------------------------------*/

        // sConfig.OCMode = TIM_OCMODE_PWM1;
        // sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
        // sConfig.OCFastMode = TIM_OCFAST_DISABLE;
        // sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        // sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        // sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
        // sConfig.Pulse = calculatePeriod (fps) * 2 - 100;

        // if (HAL_TIM_PWM_ConfigChannel (&htimSeg, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
        //         Error_Handler ();
        // }

        /*--------------------------------------------------------------------------*/

        // DMA1_Channel2->CCR &= ~DMA_CCR_EN;
        // DMA1_Channel5->CCR &= ~DMA_CCR_EN;
        // DMA1_Channel2->CNDTR = enableBuffer.size ();
        // DMA1_Channel5->CNDTR = displayBuffer.size ();
        // DMA1_Channel2->CCR |= DMA_CCR_EN;
        // DMA1_Channel5->CCR |= DMA_CCR_EN;

        /*
         * This does not start the TIM1 if it's configured in slave mode (which it is).
         * BUT it is necessary to show the PWM output on the PA8.
         */
        if (HAL_TIM_PWM_Start (&htimEn, TIM_CHANNEL_1) != HAL_OK) { // Slave TIM1
                Error_Handler ();
        }

        // HAL_Delay (10);
        // TIM1->CNT = 0;
        // TIM1->EGR |= TIM_EGR_UG;
        // TIM15->CNT = 0;
        // TIM15->EGR |= TIM_EGR_UG;

        /*
         * This strange value is configured to assure that promptly after this line
         * we would get the UP event on the TIM15, and as a consequence on the TIM1
         * as well because they are synchronized.
         */
        // TIM15->CNT = calculatePeriod (fps) * 2 - 2;

        // for (int i = 0; i < 1000; ++i) {
        //         __NOP ();
        // }

        // if (HAL_TIM_PWM_Start (&htimSeg, TIM_CHANNEL_1) != HAL_OK) { // Master TIM15
        //         Error_Handler ();
        // }

        // instance = this;
        // HAL_NVIC_SetPriority (DMA1_Channel2_3_IRQn, 3, 0);
        // HAL_NVIC_EnableIRQ (DMA1_Channel2_3_IRQn);

        // HAL_NVIC_SetPriority (DMA1_Channel4_5_6_7_IRQn, 3, 0);
        // HAL_NVIC_EnableIRQ (DMA1_Channel4_5_6_7_IRQn);

        // TODO workaround - DMA state is reset this way. I dno't know how to fix this properly.
        // Somehow running the (slightly modified) init method for the second time in a row, resolves
        // the problem (display coruption when using fps command).
        // init2 (fps);
}

/*****************************************************************************/

// extern "C" void DMA1_Channel2_3_IRQHandler ()
// {
//         // Clear the flag
//         DMA1->IFCR = DMA_FLAG_TC2;
//         DMA1->IFCR = DMA_FLAG_TE2;

//         if (DMA1->ISR & DMA_FLAG_TE2) {
//                 while (true) {
//                 }
//         }

//         // if (instance->brightness != instance->prevBrightness) {
//         //         instance->prevBrightness = instance->brightness;
//         //         TIM1->CCR1 = instance->brightnessLookup.at (instance->brightness - 1);
//         // }
// }

extern "C" void DMA1_Channel4_5_6_7_IRQHandler ()
{
        // Clear the flag
        DMA1->IFCR = DMA_FLAG_TC5;
        // DMA1->IFCR = DMA_FLAG_TE5;

        // if (DMA1->ISR & DMA_FLAG_TE5) {
        //         while (true) {
        //         }
        // }

        // if (instance->brightness != instance->prevBrightness) {
        //         instance->prevBrightness = instance->brightness;
        //         TIM1->CCR1 = instance->brightnessLookup.at (instance->brightness - 1);
        // }
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setDigit (uint8_t position, uint8_t digit)
{
        if (digit >= 48 && digit <= 57) {
                digit -= 48;
        }
        else if (digit >= 65 /*A*/ && digit <= 90 /*Z*/) {
                digit -= 55;
        }
        else if (digit >= 97 /*a*/ && digit <= 122 /*z*/) {
                digit -= 87;
        }
        else if (digit == ' ') {
                digit = SPACE_CHAR_INDEX;
        }

        int fPosition = (flip) ? (5 - position) : (position);

        if (digit == '.') {
                setDot (position, true);
        }
        else {
                uint16_t fnt = (flip) ? (flipFont (FONTS.at (digit))) : (FONTS.at (digit));
#ifdef COMMON_CATHODE
                displayBuffer.at (fPosition) = ALL_SEGMENTS | fnt; // Czarne chińskie
#else
                displayBuffer.at (fPosition) = ALL_SEGMENTS | ~fnt; // Szare KinkBright
#endif
        }

        auto fDots = (flip) ? (dots << 1) : (dots);

#ifdef COMMON_CATHODE
        if (fDots & (1 << position)) {
                displayBuffer.at (fPosition) &= ~DOT_MASK;
        }
        else {
                displayBuffer.at (fPosition) |= DOT_MASK;
        }
#else
        if (fDots & (1 << position)) {
                displayBuffer.at (fPosition) |= DOT_MASK;
        }
        else {
                displayBuffer.at (fPosition) &= ~DOT_MASK;
        }
#endif
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

        for (int i = 5; i >= 0; --i, ++factorIndexCopy) {
                auto factor = FACTORS.at (factorIndexCopy);
                setDigit (i, cntTmp % factor);
                cntTmp /= factor;
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

/**
 *
 */
void Led7SegmentDisplayDma::setBrightness (uint8_t b)
{
        // __disable_irq ();
        brightness = std::max<uint8_t> (MIN_BRIGHTNESS, b);
        brightness = std::min<uint8_t> (MAX_BRIGHTNESS, b);
        // __enable_irq ();

        TIM1->CCR1 = brightnessLookup.at (brightness - 1);
}

/****************************************************************************/

void Led7SegmentDisplayDma::setFps (unsigned int fps)
{
        fps = std::max<uint16_t> (MIN_FPS, fps);
        fps = std::min<uint16_t> (MAX_FPS, fps);
        if (prevFps != fps) {
                init (fps);
                init (fps);
                prevFps = fps;
        }
}

/****************************************************************************/

void Led7SegmentDisplayDma::recalculateBrightnessTable (unsigned int fps)
{
        auto newPeriod = calculatePeriod (fps);
        auto maxB = 3; // Max brightness
        auto step = newPeriod / (Led7SegmentDisplayDma::MAX_BRIGHTNESS + 1);

        // TODO assumes that MAX_BRIGHTNESS equals 4.            30     960     10000
        brightnessLookup.at (3) = maxB;                       // 3      3       3
        brightnessLookup.at (2) = maxB + 2 * step;            // 557    19      3
        brightnessLookup.at (1) = maxB + 3 * step;            // 834    27      3
        brightnessLookup.at (0) = maxB + 4 * step + step / 2; // 1249   39      3
}

/****************************************************************************/

void Led7SegmentDisplayDma::setFlip (bool f)
{
        if (flip != f) {
                // Reverse the text direction
                std::swap (displayBuffer.at (0), displayBuffer.at (5));
                std::swap (displayBuffer.at (1), displayBuffer.at (4));
                std::swap (displayBuffer.at (2), displayBuffer.at (3));

                // Flip every letter / digit upside down
                for (auto &e : displayBuffer) {
                        e = ALL_SEGMENTS | flipFont (e); // All dots are lit
                }

                // Display all dots. When image is flipped, draw from right to left, and shift by 1.
                for (int position = 0; position < DISPLAY_NUM; ++position) {

                        int fPosition = (f) ? (5 - position) : (position);
                        auto fDots = (f) ? (dots << 1) : (dots);

                        if (fDots & (1 << position)) {
                                displayBuffer.at (fPosition) &= ~DOT_MASK;
                        }
                        else {
                                displayBuffer.at (fPosition) |= DOT_MASK;
                        }
                }
        }

        flip = f;
}
