/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Lsm6ds3SpiBsp.h"

extern "C" void Error_Handler (void);

/*****************************************************************************/

// void Lsm6ds3SpiBsp::itConfig ()
//{
////        GPIO_InitTypeDef gpioInitStructure;

////        /* Enable INT2 GPIO clock and configure GPIO PINs to detect Interrupts */
////        ACCELERO_INT_GPIO_CLK_ENABLE ();

////        /* Configure GPIO PINs to detect Interrupts */
////        gpioInitStructure.Pin = ACCELERO_INT1_PIN | ACCELERO_INT2_PIN;
////        gpioInitStructure.Mode = GPIO_MODE_IT_RISING;
////        gpioInitStructure.Speed = GPIO_SPEED_HIGH;
////        gpioInitStructure.Pull = GPIO_PULLDOWN;
////        HAL_GPIO_Init (ACCELERO_INT_GPIO_PORT, &gpioInitStructure);

////        /* Enable and set Accelerometer INT2 to the lowest priority */
////        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn, 0x01, 0);
////        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn);

////        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn, 0x01, 0);
////        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn);
//}

/*****************************************************************************/

void Lsm6ds3SpiBsp::writeRegister (uint8_t addr, uint8_t value)
{
        spi->setNss (false);
        spi->transmit8 (WRITE_MASK & addr);
        spi->transmit8 (value);
        spi->setNss (true);
}

/*****************************************************************************/

uint8_t Lsm6ds3SpiBsp::readRegister (uint8_t addr)
{
        spi->setNss (false);
        spi->transmit8 (READ_MASK | addr);
        uint8_t ret = spi->transmit8 (0x00);
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void Lsm6ds3SpiBsp::writeRegister (uint8_t addr, uint8_t *buffer, uint16_t len)
{
        spi->setNss (false);
        spi->transmit8 (WRITE_MASK & addr);
        spi->transmit8 (buffer, len);
        spi->setNss (true);
}

/*****************************************************************************/

void Lsm6ds3SpiBsp::readRegister (uint8_t addr, uint8_t *buffer, uint16_t len)
{
        spi->setNss (false);
        spi->transmit8 (READ_MASK | addr);
        spi->transmit8 (buffer, len, buffer);
        spi->setNss (true);
}
