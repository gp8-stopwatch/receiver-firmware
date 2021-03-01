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
        static TIM_HandleTypeDef htim{};

        htim.Instance = TIM15;
        htim.Init.Prescaler = 48 - 1;
        htim.Init.Period = 200 - 1;
        htim.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim.Init.RepetitionCounter = 0;
        htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        __HAL_RCC_TIM15_CLK_ENABLE ();

        /****************************************************************************/

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

        /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
        // HAL_DMA_RegisterCallback (&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
        // HAL_DMA_RegisterCallback (&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

        // /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
        // /* Set Interrupt Group Priority */
        // HAL_NVIC_SetPriority (DMA_INSTANCE_IRQ, 0, 0);

        // /* Enable the DMA global Interrupt */
        // HAL_NVIC_EnableIRQ (DMA_INSTANCE_IRQ);

        static std::array<uint32_t, DISPLAY_NUM> displayBuffer{0b0000'0000'1010'1010'0000'0000'0000'0000,
                                                               0b0000'0000'0000'0000'0000'0000'1111'1111};

        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA transfer */
        /* Enable All the DMA interrupts */
        if (HAL_DMA_Start /* _IT */ (&dmaHandle, reinterpret_cast<uint32_t> (displayBuffer.data ()), reinterpret_cast<uint32_t> (GPIOA->BSRR),
                                     displayBuffer.size ())
            != HAL_OK) {
                Error_Handler ();
        }

        __HAL_TIM_ENABLE_DMA (&htim, TIM_DMA_UPDATE);

        if (HAL_TIM_Base_Start (&htim) != HAL_OK) {
                Error_Handler ();
        }

        while (true) {
        }
}

/*****************************************************************************/

void Led7SegmentDisplayDma::refresh ()
{
        if (brightness == 0) {
                return;
        }

        // Change a digit at the beginning of each "brightnessCycle" and lit it up.
        if (brightnessCycle == 0) {
                turnDisplay (currentDigit, false);
                ++currentDigit;
                currentDigit %= 6;
                outputDigit (currentDigit);
                turnDisplay (currentDigit, true);
        }

        if (brightnessCycle >= brightness) {
                turnDisplay (currentDigit, false);
        }

        ++brightnessCycle;
        brightnessCycle %= MAX_BRIGHTNESS + 1;
}

/*****************************************************************************/

void Led7SegmentDisplayDma::setDigit (uint8_t position, uint8_t digit)
{
        if (digit >= 0 && digit <= 0xf) {
                digits[position] = digit;
        }
        else if (digit >= 48 && digit <= 57) {
                digits[position] = digit - 48;
        }
        else if (digit >= 65 /*A*/ && digit <= 90 /*Z*/) {
                digits[position] = digit - 55;
        }
        else if (digit >= 97 /*a*/ && digit <= 122 /*z*/) {
                digits[position] = digit - 87;
        }
        else if (digit == '.') {
                setDot (position, true);
        }
        else if (digit == ' ') {
                digits[position] = SPACE_CHAR;
        }
}

/****************************************************************************/

void Led7SegmentDisplayDma::outputDigit (uint8_t position)
{
        if (flip) {
                position = DISPLAY_NUM - 1 - position;
        }

        uint8_t font = fonts[digits[position]];

        if (flip) {
                font = flipFont (font);
        }

        for (uint8_t seg = 0; seg < 7; ++seg) {
                *segment.at (seg) = !CA ^ bool (font & (1 << seg));
        }

        *segment.at (7) = !CA ^ bool (dots & (1 << position));
}

/*****************************************************************************/

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
                digits[i] = SPACE_CHAR;
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
