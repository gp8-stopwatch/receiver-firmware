/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Lis3dshSpiBsp.h"

extern "C" void Error_Handler (void);

Lis3dshSpiBsp *Lis3dshSpiBsp::singleton ()
{
        static Lis3dshSpiBsp bsp;
        return &bsp;
}

/*****************************************************************************/

void Lis3dshSpiBsp::init ()
{
        GPIO_InitTypeDef GPIO_InitStructure;
        __GPIOB_CLK_ENABLE ();
        GPIO_InitStructure.Pin = ACCELERO_CS_PIN;
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
        HAL_GPIO_Init (ACCELERO_CS_GPIO_PORT, &GPIO_InitStructure);
        ACCELERO_CS_HIGH ();

        __HAL_RCC_SPI2_CLK_ENABLE ();
        __GPIOB_CLK_ENABLE ();
        GPIO_InitStructure.Pin = (GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
        GPIO_InitStructure.Alternate = GPIO_AF0_SPI2;
        HAL_GPIO_Init (GPIOB, &GPIO_InitStructure);

        accelSpiHandle.Instance = SPI2;
        accelSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
        accelSpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
        accelSpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
        accelSpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
        accelSpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        accelSpiHandle.Init.CRCPolynomial = 7;
        accelSpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
        accelSpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
        accelSpiHandle.Init.NSS = SPI_NSS_SOFT;
        accelSpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;
        accelSpiHandle.Init.Mode = SPI_MODE_MASTER;

        HAL_SPI_Init (&accelSpiHandle);
}

/*****************************************************************************/

void Lis3dshSpiBsp::itConfig ()
{
        GPIO_InitTypeDef gpioInitStructure;

        /* Enable INT2 GPIO clock and configure GPIO PINs to detect Interrupts */
        ACCELERO_INT_GPIO_CLK_ENABLE ();

        /* Configure GPIO PINs to detect Interrupts */
        gpioInitStructure.Pin = ACCELERO_INT1_PIN | ACCELERO_INT2_PIN;
        gpioInitStructure.Mode = GPIO_MODE_IT_RISING;
        gpioInitStructure.Speed = GPIO_SPEED_HIGH;
        gpioInitStructure.Pull = GPIO_PULLDOWN;
        HAL_GPIO_Init (ACCELERO_INT_GPIO_PORT, &gpioInitStructure);

        /* Enable and set Accelerometer INT2 to the lowest priority */
        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn, 0x01, 0);
        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn);

        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn, 0x01, 0);
        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn);
}

/*****************************************************************************/

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received.
  *         from the SPI bus.
  * @param  Byte: Byte send.
  * @retval The received byte value
  */
uint8_t Lis3dshSpiBsp::spiWriteRead (uint8_t byte)
{
        uint8_t receivedbyte = 0;

        if (HAL_SPI_TransmitReceive (&accelSpiHandle, (uint8_t *)&byte, (uint8_t *)&receivedbyte, 1, 500) != HAL_OK) {
                Error_Handler ();
        }

        return receivedbyte;
}

/*****************************************************************************/

void Lis3dshSpiBsp::write (uint8_t *buffer, uint8_t addr, uint16_t len)
{
        /* Configure the MS bit:
           - When 0, the address will remain unchanged in multiple read/write commands.
           - When 1, the address will be auto incremented in multiple read/write commands.
        */
        if (len > 0x01) {
                addr |= (uint8_t)MULTIPLEBYTE_CMD;
        }
        /* Set chip select Low at the start of the transmission */
        ACCELERO_CS_LOW ();

        /* Send the Address of the indexed register */
        spiWriteRead (addr);

        /* Send the data that will be written into the device (MSB First) */
        while (len >= 0x01) {
                spiWriteRead (*buffer);
                --len;
                ++buffer;
        }

        /* Set chip select High at the end of the transmission */
        ACCELERO_CS_HIGH ();
}

/*****************************************************************************/

void Lis3dshSpiBsp::read (uint8_t *buffer, uint8_t addr, uint16_t len)
{
        if (len > 0x01) {
                addr |= (uint8_t) (READWRITE_CMD | MULTIPLEBYTE_CMD);
        }
        else {
                addr |= (uint8_t)READWRITE_CMD;
        }
        /* Set chip select Low at the start of the transmission */
        ACCELERO_CS_LOW ();

        /* Send the Address of the indexed register */
        spiWriteRead (addr);

        /* Receive the data that will be read from the device (MSB First) */
        while (len > 0x00) {
                /* Send dummy byte (0x00) to generate the SPI clock to ACCELEROMETER (Slave device) */
                *buffer = spiWriteRead (0xff);
                --len;
                ++buffer;
        }

        /* Set chip select High at the end of the transmission */
        ACCELERO_CS_HIGH ();
}

/*****************************************************************************/

extern "C" void ACCELERO_INT1_HANDLER ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (ACCELERO_INT1_PIN);
        Lis3dshSpiBsp *bsp = Lis3dshSpiBsp::singleton ();
        if (bsp->lis3dshBspListener) {
                bsp->lis3dshBspListener->onInterrupt1 ();
        }
}

/*****************************************************************************/

extern "C" void ACCELERO_INT2_HANDLER ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (ACCELERO_INT2_PIN);
        Lis3dshSpiBsp *bsp = Lis3dshSpiBsp::singleton ();
        if (bsp->lis3dshBspListener) {
                bsp->lis3dshBspListener->onInterrupt2 ();
        }
}
