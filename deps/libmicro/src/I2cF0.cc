/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "I2c.h"
#include <cstring>

/* Read/Write command */
#define READWRITE_CMD ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD ((uint8_t)0x40)

#include "Debug.h"

/*****************************************************************************/

I2c *I2c::i2c1;
I2c *I2c::i2c2;

/*****************************************************************************/

I2c::I2c (I2C_TypeDef *hi2c)
    : callback (nullptr),
      state (SLAVE_ADDR_LISTEN),
      currentAddress (0),
      txBuffer (nullptr),
      txPointer (nullptr),
      txToSend (0),
      txRemaining (0),
      rxPointer (rxBuffer)
{
        if (hi2c == I2C1) {
                i2c1 = this;
        }
        else if (hi2c == I2C2) {
                i2c2 = this;
        }

        memset (&i2cHandle, 0, sizeof (i2cHandle));

        // TODO move to some method.
        // TODO Potrzebne? Necessary?
        // RCC_PeriphCLKInitTypeDef rccInit;
        // rccInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        // rccInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
        // HAL_RCCEx_PeriphCLKConfig (&rccInit);

        i2cHandle.Instance = hi2c;
        clkEnable ();

#ifdef LIB_MICRO_STM32F4
        i2cHandle.Init.ClockSpeed = 400000;
        i2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
        i2cHandle.Init.Timing = 0x2000090E; // 100kHz
                                            // i2cHandle.Init.Timing = 0;
#endif
        i2cHandle.Init.OwnAddress1 = 0x3f << 1;
        i2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        i2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        i2cHandle.Init.OwnAddress2 = 0x0;
        i2cHandle.Init.OwnAddress2Masks = 0;
        i2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        i2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

        if (HAL_I2C_Init (&i2cHandle) != HAL_OK) {
                Error_Handler ();
        }

        HAL_I2CEx_ConfigAnalogFilter (&i2cHandle, I2C_ANALOGFILTER_ENABLE);

        __HAL_I2C_ENABLE_IT (&i2cHandle,
                             I2C_IT_ERRI | /*I2C_IT_TCI |*/ I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ADDRI /*| I2C_IT_RXI | I2C_IT_TXI*/);
}

/*****************************************************************************/

extern "C" void I2C1_IRQHandler () { I2c::i2c1->slaveIrq (I2c::i2c1); }
extern "C" void I2C2_IRQHandler () { I2c::i2c2->slaveIrq (I2c::i2c2); }

/*****************************************************************************/

void I2c::slaveIrq (I2c *i2c)
{
#ifdef ISR_PRINT
        Debug *d = Debug::singleton ();
#endif
        I2C_HandleTypeDef *hi2c = &i2c->i2cHandle;
        I2C_TypeDef *i2ci = hi2c->Instance;
        uint32_t itFlags = i2ci->ISR;
        uint32_t itSources = i2ci->CR1;
        uint32_t errorCode = 0;

        // NACK detected
        if ((itFlags & I2C_ISR_NACKF) && (itSources & I2C_IT_NACKI)) {
#ifdef ISR_PRINT
                d->print ("N1\n");
#endif
                // Clear IRQ.
                i2ci->ICR = I2C_ISR_NACKF;

                // Master does not want us to send any more data.
                if (state == SLAVE_BYTE_TX && txRemaining) {
                        __HAL_I2C_DISABLE_IT (&i2cHandle, I2C_IT_TXI);
                        size_t tmpLen = txToSend;
                        txToSend = txRemaining = 0;
                        callback->onTxComplete (currentAddress, txBuffer, tmpLen);
                }

                state = SLAVE_ADDR_LISTEN;
#ifdef ISR_PRINT
                d->print ("N2\n");
#endif
        }
        // RX not empty, new data (1 byte) available.
        else if ((itFlags & I2C_ISR_RXNE) && (itSources & I2C_IT_RXI)) {
#ifdef ISR_PRINT
                d->print ("R1\n");
#endif
                if (state != SLAVE_BYTE_RX) {
#ifdef ISR_PRINT
                        d->print ("R3\n");
#endif
                        return;
                }

                do {
                        if (rxReceived < RX_BUFFER_SIZE) {
                                *rxPointer++ = static_cast<uint8_t> (i2ci->RXDR);
                                ++rxReceived;
                        }
                        else {
                                // send NACK when rxBuffer is full.
                                i2ci->CR2 |= I2C_CR2_NACK;
                                break;
                        }
                } while (i2ci->ISR & I2C_ISR_RXNE);

#ifdef ISR_PRINT
                d->print ("R2\n");
#endif
        }
        // Address matched (slave mode)
        else if ((itFlags & I2C_ISR_ADDR) && (itSources & I2C_IT_ADDRI)) {
#ifdef ISR_PRINT
                d->print ("A1\n");
#endif
                if (state == SLAVE_BYTE_RX && rxReceived) {
                        __HAL_I2C_DISABLE_IT (&i2cHandle, I2C_IT_RXI);
                        callback->onRxComplete (currentAddress, rxBuffer, rxReceived);
                        state = SLAVE_ADDR_LISTEN;
                        rxReceived = 0;
                        rxPointer = rxBuffer;
                }
                else if (state == SLAVE_BYTE_TX && txRemaining) {
                        __HAL_I2C_DISABLE_IT (&i2cHandle, I2C_IT_TXI);
                        size_t tmpLen = txToSend;
                        txToSend = txRemaining = 0;
                        callback->onTxComplete (currentAddress, txBuffer, tmpLen);
                        state = SLAVE_ADDR_LISTEN;
                }

                if (state == SLAVE_ADDR_LISTEN) {

                        // If true, means that some master wants to read from us.
                        bool slaveTransmit = (i2ci->ISR & I2C_ISR_DIR) >> 16;

                        if (slaveTransmit) {
                                __HAL_I2C_ENABLE_IT (&i2cHandle, I2C_IT_TXI);
                                state = SLAVE_BYTE_TX;
                        }
                        else {
                                __HAL_I2C_ENABLE_IT (&i2cHandle, I2C_IT_RXI);
                                rxReceived = 0;
                                rxPointer = rxBuffer;
                                state = SLAVE_BYTE_RX;
                        }

                        // Address which came from a master and was detected as this slave address.
                        uint16_t slaveAddr = (i2ci->ISR & I2C_ISR_ADDCODE) >> 17;

                        // I2C_OAR1 (Own address 1 register) is enabled.
                        if (i2ci->OAR1 & I2C_OAR1_OA1EN) {
                                // 7bit address in I2C_OAR1
                                if (!(i2ci->OAR1 & I2C_OAR1_OA1MODE)) {
                                        currentAddress = slaveAddr;
                                }
                                // 10bit address in I2C_OAR1
                                else {
                                        // TODO
                                }
                        }

                        // I2C_OAR2 (Own address 2 register) is enabled. It is always 7 bit
                        if (i2ci->OAR2 & I2C_OAR2_OA2EN) {
                                currentAddress = slaveAddr;
                        }

                        // uint16_t ownadd1code = I2C_GET_OWN_ADDRESS1 (hi2c);
                        // uint16_t ownadd2code = I2C_GET_OWN_ADDRESS2 (hi2c);
#ifdef ISR_PRINT
                        d->print ("A2\n");
                        d->print (currentAddress);
                        d->print (",");
                        d->print (slaveTransmit);
                        d->print ("\n");
#endif
                }

                // This stretch is released when the ADDR flag is cleared by software setting the ADDRCF bit.
                hi2c->Instance->ICR = I2C_ISR_ADDR;
#ifdef ISR_PRINT
                d->print ("A3\n");
#endif
        }
        // TX empty
        else if ((itFlags & I2C_ISR_TXIS) && (itSources & I2C_IT_TXI)) {
#ifdef ISR_PRINT
                d->print ("T1\n");
#endif
                if (!txRemaining || state != SLAVE_BYTE_TX) {
                        __HAL_I2C_DISABLE_IT (&i2cHandle, I2C_IT_TXI);
#ifdef ISR_PRINT
                        d->print ("T3\n");
#endif
                }
                else if (txRemaining) {
#ifdef ISR_PRINT
                        d->print (".\n");
#endif
                        /* Write data to TXDR */
                        i2ci->TXDR = (*txPointer++);
                        --txRemaining;
                }

                if (txToSend && !txRemaining) {
                        state = SLAVE_ADDR_LISTEN;
                        size_t tmpLen = txToSend;
                        txToSend = txRemaining = 0;
                        callback->onTxComplete (currentAddress, txBuffer, tmpLen);
                }

#ifdef ISR_PRINT
//                d->print ("T2\n");
#endif
        }

        // STOP detected.
        if ((itFlags & I2C_ISR_STOPF) && (itSources & I2C_IT_STOPI)) {
#ifdef ISR_PRINT
                d->print ("S1\n");
#endif

                // Clear STOPF and ADDR IRQs.
                i2ci->ICR = I2C_ISR_STOPF;
                __HAL_I2C_DISABLE_IT (&i2cHandle, I2C_IT_TXI | I2C_IT_RXI);

                //                 flushTx ();

                if (txRemaining != 0) {
                        errorCode |= HAL_I2C_ERROR_AF;
                }

                if (state == SLAVE_BYTE_RX && rxReceived) {
                        callback->onRxComplete (currentAddress, rxBuffer, rxReceived);
                        rxReceived = 0;
                        rxPointer = rxBuffer;
                }
                // else if (state == SLAVE_BYTE_TX && txRemaining) {
                //         size_t tmpLen = txToSend;
                //         txToSend = txRemaining = 0;
                //         callback->onTxComplete (currentAddress, txBuffer, tmpLen);
                // }

                state = SLAVE_ADDR_LISTEN;
#ifdef ISR_PRINT
                d->print ("S2\n");
#endif
        }

        if (itSources & I2C_IT_ERRI) {

                // I2C Bus error interrupt occurred
                if (itFlags & I2C_FLAG_BERR) {
                        errorCode |= HAL_I2C_ERROR_BERR;
                        __HAL_I2C_CLEAR_FLAG (hi2c, I2C_FLAG_BERR);
                }

                // I2C Over-Run/Under-Run interrupt occurred
                if (itFlags & I2C_FLAG_OVR) {
                        errorCode |= HAL_I2C_ERROR_OVR;
                        __HAL_I2C_CLEAR_FLAG (hi2c, I2C_FLAG_OVR);
                }

                // I2C Arbitration Loss error interrupt occurred
                if (itFlags & I2C_FLAG_ARLO) {
                        errorCode |= HAL_I2C_ERROR_ARLO;
                        __HAL_I2C_CLEAR_FLAG (hi2c, I2C_FLAG_ARLO);
                }

                if (itFlags & I2C_FLAG_TIMEOUT) {
                        errorCode |= HAL_I2C_ERROR_TIMEOUT;
                        __HAL_I2C_CLEAR_FLAG (hi2c, I2C_FLAG_TIMEOUT);
                }

                if (errorCode) {
                        callback->onI2cError (errorCode);
                }
        }
}

/*****************************************************************************/

void I2c::read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        //        while (HAL_I2C_Master_Sequential_Transmit_IT (&I2cHandle, (uint16_t)devAddr, &regAddr, 1, I2C_FIRST_FRAME) != HAL_OK) {
        while (HAL_I2C_Master_Transmit (&i2cHandle, (uint16_t)devAddr << 1, &regAddr, 1, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }

        while (HAL_I2C_GetState (&i2cHandle) != HAL_I2C_STATE_READY) {
        }

        //        while (HAL_I2C_Master_Sequential_Receive_IT (&I2cHandle, (uint16_t)devAddr, data, length, I2C_LAST_FRAME) != HAL_OK) {
        while (HAL_I2C_Master_Receive (&i2cHandle, (uint16_t)devAddr << 1, data, length, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }

        while (HAL_I2C_GetState (&i2cHandle) != HAL_I2C_STATE_READY) {
        }
}

/*****************************************************************************/

void I2c::write (uint8_t devAddr, uint8_t regAddr, const uint8_t *data, size_t length, uint16_t timeout)
{
        uint8_t tmp[128];
        tmp[0] = regAddr;

        if (length <= 127) {
                memcpy (tmp + 1, data, length);
        }
        else {
                Error_Handler ();
        }

        while (HAL_I2C_Master_Transmit (&i2cHandle, (uint16_t)devAddr << 1, tmp, length + 1, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }
}

/*****************************************************************************/

bool I2c::slaveWrite (uint8_t const *data, size_t length)
{
        if (state == SLAVE_BYTE_TX) {
                return false;
        }

        txBuffer = txPointer = data;
        txToSend = txRemaining = length;
        return true;
}

/*****************************************************************************/

void I2c::reset ()
{
        i2cHandle.Instance->CR1 &= ~I2C_CR1_PE;
        if (!(i2cHandle.Instance->CR1 | I2C_CR1_PE)) {
                i2cHandle.Instance->CR1 |= I2C_CR1_PE;
        }
}

void I2c::flushTx ()
{
#ifdef ISR_PRINT
        Debug::singleton ()->print ("F\n");
#endif
        if (i2cHandle.Instance->ISR & I2C_FLAG_TXIS) {
                i2cHandle.Instance->TXDR = 0x00U;
        }

        // Flush TX register if not empty
        if (i2cHandle.Instance->ISR & I2C_FLAG_TXE) {
                i2cHandle.Instance->ISR |= I2C_FLAG_TXE;
        }
}

/*****************************************************************************/

void I2c::clkEnable ()
{
        if (i2cHandle.Instance == I2C1) {
                __HAL_RCC_I2C1_CLK_ENABLE ();
        }
        else if (i2cHandle.Instance == I2C2) {
                __HAL_RCC_I2C2_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void I2c::clkDisable ()
{
        if (i2cHandle.Instance == I2C1) {
                __HAL_RCC_I2C1_CLK_DISABLE ();
        }
        else if (i2cHandle.Instance == I2C2) {
                __HAL_RCC_I2C2_CLK_DISABLE ();
        }
}
