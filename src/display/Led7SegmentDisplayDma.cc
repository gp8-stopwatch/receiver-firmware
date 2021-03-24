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

/****************************************************************************/

uint16_t flipFont (uint16_t font)
{
        return (font & 0x06) << 5 | (font & 0xc0) >> 5 | (font & 0x20) | (font & 0x08) >> 3 | (font & 0x01) << 3 | (font & 0x100);
}

/****************************************************************************/

Led7SegmentDisplayDma::Led7SegmentDisplayDma ()
{
        // This method fills in the two buffers which would got transferred by the DMA later on.
        // setBrightness (1);

        /*--------------------------------------------------------------------------*/
        /* Enable (common pin) timer & DMA. This enables individual displays.       */
        /*--------------------------------------------------------------------------*/

        TIM_HandleTypeDef htim{};

        htim.Instance = TIM1;
        htim.Init.Prescaler = calculatePrescaler (FPS) - 1;
        htim.Init.Period = PERIOD - 1;
        htim.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED3;
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim.Init.RepetitionCounter = 0;
        htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

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

        __HAL_LINKDMA (&htim, hdma[TIM_DMA_ID_CC1], dmaHandle);

        if (HAL_DMA_Init (htim.hdma[TIM_DMA_ID_CC1]) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_PWM_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        /*
         * enableBuffer (24 elements, each 32 bit) is transferred constatntly (thanks
         * to the DMA circular mode) to the GPIOB->BSRR. First 16 bits of every element
         * of this buffer is for clearing, and less signifficat 16 bits are for setting.
         *
         * Enable buffer is 4 (MAX_BRIGHTNESS) times bigger than the displayBuffer (which has only 6 elements).
         * This is why TIM1 has to update 4 times faster. This is for brightness setting impl.
         */
        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (enableBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOB->BSRR),
                           enableBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        __HAL_TIM_ENABLE_DMA (&htim, TIM_DMA_CC1);

        /*--------------------------------------------------------------------------*/

        TIM_OC_InitTypeDef sConfig{};
        sConfig.OCMode = TIM_OCMODE_PWM1;
        sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfig.OCFastMode = TIM_OCFAST_DISABLE;
        sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
        // sConfig.Pulse = PERIOD / 2 - 1;
        sConfig.Pulse = 3; // <- this controlls the brightness. Low value means high intensity. Correct value is 3-48 when PERIOD is 50 and
                           // PRESCALER 480. If the two timers were synchronized, then we could achieve better accuracy and higher brightness
                           // range. The accuracy drops when timers run faster (i.e. when PRESCALER is lower).

        if (HAL_TIM_PWM_ConfigChannel (&htim, &sConfig, TIM_CHANNEL_1) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_TIM_PWM_Start (&htim, TIM_CHANNEL_1) != HAL_OK) {
                Error_Handler ();
        }

        /*--------------------------------------------------------------------------*/
        /* Segment timer & DMA. This drives single segments.                        */
        /*--------------------------------------------------------------------------*/

        /*
         * Almost all the settings of TIM16 and DMA (the only DMA IP here)
         * are the same. I'm reusing the init strucures.
         */
        htim.Instance = TIM16;
        htim.Init.Prescaler = calculatePrescaler (FPS) - 1;
        // htim.Init.Period = PERIOD * 2 - 1;
        htim.Init.Period = 98 - 1; // This was found exparimentally, and I don't really understand why there's 98 instead of 100.

        __HAL_RCC_TIM16_CLK_ENABLE ();
        dmaHandle.Instance = DMA1_Channel3;
        __HAL_LINKDMA (&htim, hdma[TIM_DMA_ID_UPDATE], dmaHandle);

        if (HAL_DMA_Init (htim.hdma[TIM_DMA_ID_UPDATE]) != HAL_OK) {
                Error_Handler ();
        }

        // PWM because of this OC channel used as described above.
        if (HAL_TIM_Base_Init (&htim) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_DMA_Start (&dmaHandle, reinterpret_cast<uint32_t> (displayBuffer.data ()), reinterpret_cast<uint32_t> (&GPIOA->BSRR),
                           displayBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        __HAL_TIM_ENABLE_DMA (&htim, TIM_DMA_UPDATE);

        /*--------------------------------------------------------------------------*/

        if (HAL_TIM_Base_Start (&htim) != HAL_OK) {
                Error_Handler ();
        }

        TIM1->CNT = 0;
        TIM16->CNT = 0;
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
                displayBuffer.at (fPosition) = ALL_SEGMENTS | fnt;
        }

        auto fDots = (flip) ? (dots << 1) : (dots);

        if (fDots & (1 << position)) {
                displayBuffer.at (fPosition) &= ~DOT_MASK;
        }
        else {
                displayBuffer.at (fPosition) |= DOT_MASK;
        }
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

/****************************************************************************/

/**
 * This fills the enableBuffer so the enable pins are driven in a sequence.
 * DMA is transferring the buffer in circular mode. Below are examples of
 * how would the buffer look to obtain different brightnes intensity on the
 * screen:
 *
 * Brightnes : 4 (max)      3                  2                  1
 * en. disp. 0         en. disp. 0        en. disp. 0        en. disp. 0
 * en. disp. 0         en. disp. 0        en. disp. 0        disable all
 * en. disp. 0         en. disp. 0        disable all        disable all
 * en. disp. 0         disable all        disable all        disable all
 * en. disp. 1         en. disp. 1        en. disp. 1        en. disp. 1
 * en. disp. 1         en. disp. 1        en. disp. 1        disable all
 * en. disp. 1         en. disp. 1        disable all        disable all
 * en. disp. 1         disable all        disable all        disable all
 * en. disp. 2         en. disp. 2        en. disp. 2        en. disp. 2
 * en. disp. 2         en. disp. 2        en. disp. 2        disable all
 * ...                 ...                ...                ...
 *
 * The table above gives the general idea, but due to synchronization issues
 * (synchr. between TIM1 and TIM16) the sequence is a little bit shifted (modulo 24).
 * True sequence can be seen in the enableBuffer initialization, but the duty
 * cycle idea is the same.
 */
void Led7SegmentDisplayDma::setBrightness (uint8_t b)
{
        brightness = std::max<uint8_t> (MIN_BRIGHTNESS, b);
        brightness = std::min<uint8_t> (MAX_BRIGHTNESS, b);

        GPIOB->BSRR = ALL_ENABLE_OFF;

        // TIM1->CR1 &= ~TIM_CR1_CEN;
        // TIM16->CR1 &= ~TIM_CR1_CEN;

        DMA1_Channel2->CCR &= ~DMA_CCR_EN;
        DMA1_Channel3->CCR &= ~DMA_CCR_EN;

        DMA1->IFCR = DMA_FLAG_GL2 | DMA_FLAG_GL3;

        DMA1_Channel2->CNDTR = enableBuffer.size ();  // 12
        DMA1_Channel3->CNDTR = displayBuffer.size (); // 6

        DMA1_Channel3->CCR |= DMA_CCR_EN;
        DMA1_Channel2->CCR |= DMA_CCR_EN;

        constexpr std::array<uint8_t, MAX_BRIGHTNESS> brightnessLookup = {48, 33, 18, 3};
        TIM1->CCR1 = brightnessLookup.at (brightness - 1);
        // TIM1->CCR1 = 33;
        TIM1->CNT = 0;
        TIM16->CNT = 0;

        // TIM1->CR1 |= TIM_CR1_CEN;
        // TIM16->CR1 |= TIM_CR1_CEN;
}

/****************************************************************************/

void Led7SegmentDisplayDma::setFps (unsigned int fps)
{
        TIM1->PSC = calculatePrescaler (fps);
        TIM16->PSC = calculatePrescaler (fps);
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
