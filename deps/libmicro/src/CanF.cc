/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Can.h"
#include <Debug.h>
#include <ErrorHandler.h>
#include <cstdlib>
#include <cstring>

/*****************************************************************************/

Can *Can::can;

extern "C" void CEC_CAN_IRQHandler ()
{
        CAN_HandleTypeDef *hcan = &Can::can->canHandle;
        CAN_TypeDef *ican = hcan->Instance;
        CanFrame frame;
        Debug *d = Debug::singleton ();

        /* Check End of reception flag for FIFO0 */
        if ((__HAL_CAN_GET_IT_SOURCE (hcan, CAN_IT_FMP0)) && (__HAL_CAN_MSG_PENDING (hcan, CAN_FIFO0) != 0)) {
                CAN_FIFOMailBox_TypeDef &mailbox = hcan->Instance->sFIFOMailBox[CAN_FIFO0];

                if (((uint8_t)0x04U & mailbox.RIR) == CAN_ID_STD) {
                        frame.id = 0x000007FFU & (mailbox.RIR >> 21U);
                        frame.extended = false;
                }
                else {
                        frame.id = 0x1FFFFFFFU & (mailbox.RIR >> 3U);
                        frame.extended = true;
                }

                frame.dlc = (uint8_t)0x0FU & mailbox.RDTR;

                frame.data[0] = (uint8_t)0xFFU & mailbox.RDLR;
                frame.data[1] = (uint8_t)0xFFU & (mailbox.RDLR >> 8U);
                frame.data[2] = (uint8_t)0xFFU & (mailbox.RDLR >> 16U);
                frame.data[3] = (uint8_t)0xFFU & (mailbox.RDLR >> 24U);
                frame.data[4] = (uint8_t)0xFFU & mailbox.RDHR;
                frame.data[5] = (uint8_t)0xFFU & (mailbox.RDHR >> 8U);
                frame.data[6] = (uint8_t)0xFFU & (mailbox.RDHR >> 16U);
                frame.data[7] = (uint8_t)0xFFU & (mailbox.RDHR >> 24U);

                __HAL_CAN_FIFO_RELEASE (hcan, CAN_FIFO0);

                if (Can::can->callback) {
                        Can::can->callback->onCanNewFrame (frame);
                }
        }

        if (__HAL_CAN_GET_IT_SOURCE (hcan, CAN_IT_TME)) {
                if ((__HAL_CAN_TRANSMIT_STATUS (hcan, CAN_TXMAILBOX_0)) || (__HAL_CAN_TRANSMIT_STATUS (hcan, CAN_TXMAILBOX_1))
                    || (__HAL_CAN_TRANSMIT_STATUS (hcan, CAN_TXMAILBOX_2))) {
                        d->print ("TME\n");
                }
        }

        if ((__HAL_CAN_GET_IT_SOURCE (hcan, CAN_IT_FMP1)) && (__HAL_CAN_MSG_PENDING (hcan, CAN_FIFO1) != 0)) {
                d->print ("FMP1\n");
        }

        uint32_t errorCode = HAL_CAN_ERROR_NONE;

        /*
         * ERRI : Error interrupt. This bit is set by hardware when a bit of the CAN_ESR has been set on error detection and
         * the corresponding interrupt in the CAN_IER is enabled. Setting this bit generates a status
         * change interrupt if the ERRIE bit in the CAN_IER register is set.
         * This bit is cleared by software.
         */
        if (ican->MSR & CAN_MSR_ERRI) {
                d->print ("ERRI\n");

                if (ican->ESR & CAN_ESR_EWGF) {
                        errorCode |= HAL_CAN_ERROR_EWG;
                }

                if (ican->ESR & CAN_ESR_EPVF) {
                        errorCode |= HAL_CAN_ERROR_EPV;
                }

                if (ican->ESR & CAN_ESR_BOFF) {
                        errorCode |= HAL_CAN_ERROR_BOF;
                }

                uint8_t lec = ican->ESR & CAN_ESR_LEC;
                switch (lec) {
                case (CAN_ESR_LEC_0):
                        /* Set CAN error code to STF error */
                        errorCode |= HAL_CAN_ERROR_STF;
                        break;
                case (CAN_ESR_LEC_1):
                        /* Set CAN error code to FOR error */
                        errorCode |= HAL_CAN_ERROR_FOR;
                        break;
                case (CAN_ESR_LEC_1 | CAN_ESR_LEC_0):
                        /* Set CAN error code to ACK error */
                        errorCode |= HAL_CAN_ERROR_ACK;
                        break;
                case (CAN_ESR_LEC_2):
                        /* Set CAN error code to BR error */
                        errorCode |= HAL_CAN_ERROR_BR;
                        break;
                case (CAN_ESR_LEC_2 | CAN_ESR_LEC_0):
                        /* Set CAN error code to BD error */
                        errorCode |= HAL_CAN_ERROR_BD;
                        break;
                case (CAN_ESR_LEC_2 | CAN_ESR_LEC_1):
                        /* Set CAN error code to CRC error */
                        errorCode |= HAL_CAN_ERROR_CRC;
                        break;
                default:
                        break;
                }

                if (lec) {
                        ican->ESR &= ~(CAN_ESR_LEC);
                }

                /* Clear ERRI Flag */
                ican->MSR |= CAN_MSR_ERRI;
        }

        /* Call the Error call Back in case of Errors */
        if (errorCode != HAL_CAN_ERROR_NONE) {
#if 1
                d->print ("Error : ");
                d->print (errorCode);
                d->print (" : ");

                if (errorCode & HAL_CAN_ERROR_EWG) {
                        d->print ("HAL_CAN_ERROR_EWG (Error warning) ");
                }
                if (errorCode & HAL_CAN_ERROR_EPV) {
                        d->print ("HAL_CAN_ERROR_EPV (Error passive) ");
                }
                if (errorCode & HAL_CAN_ERROR_BOF) {
                        d->print ("HAL_CAN_ERROR_BOF (Bus off)");
                }
                if (errorCode & HAL_CAN_ERROR_STF) {
                        d->print ("HAL_CAN_ERROR_STF (Stuff) ");
                }
                if (errorCode & HAL_CAN_ERROR_FOR) {
                        d->print ("HAL_CAN_ERROR_FOR (Form) ");
                }
                if (errorCode & HAL_CAN_ERROR_ACK) {
                        d->print ("HAL_CAN_ERROR_ACK ");
                }
                if (errorCode & HAL_CAN_ERROR_BR) {
                        d->print ("HAL_CAN_ERROR_BR (Bit recessive) ");
                }
                if (errorCode & HAL_CAN_ERROR_BD) {
                        d->print ("HAL_CAN_ERROR_BD (LEC Dominant) ");
                }
                if (errorCode & HAL_CAN_ERROR_CRC) {
                        d->print ("HAL_CAN_ERROR_CRC (LEC Transfer) ");
                }

                d->print (". REC=");
                d->print ((hcan->Instance->ESR & 0xFF000000) >> 24);
                d->print (". TEC=");
                d->print (hcan->Instance->ESR & 0x00FF0000 >> 16);
                d->print (". BOF=");
                d->print (hcan->Instance->ESR & 0b100);
                d->print (". EPVF=");
                d->print (hcan->Instance->ESR & 0b010);
                d->print (". EWGF=");
                d->print (hcan->Instance->ESR & 0b001);
                d->print ("\n");
#endif
                /* Call Error callback function */
                if (Can::can->callback) {
                        Can::can->callback->onCanError (errorCode);
                }
        }

        if (hcan->Instance->RF0R & 0b10000) {
                d->print ("OVR\n");
        }

        if (hcan->Instance->RF0R & 0b1000) {
                d->print ("FULL\n");
        }

        if (ican->MSR & CAN_MSR_SLAKI && ican->IER & CAN_IER_SLKIE) {
                d->print ("SLAK\n");
        }

        if (ican->MSR & CAN_MSR_WKUI && ican->IER & CAN_IER_WKUIE) {
                d->print ("WKUI\n");
        }
}

/*****************************************************************************/

Can::Can (ICanCallback *callback, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2) : callback (callback)
{
        can = this;
        memset (&canHandle.Init, 0, sizeof (canHandle.Init));
        canHandle.Lock = HAL_UNLOCKED;
        canHandle.State = HAL_CAN_STATE_RESET;
        canHandle.ErrorCode = 0;
        canHandle.Instance = CAN;
        clkEnable ();
        reset ();

        static CanTxMsgTypeDef txMessage;
        static CanRxMsgTypeDef rxMessage;
        canHandle.pTxMsg = &txMessage;
        canHandle.pRxMsg = &rxMessage;

        canHandle.Init.TTCM = DISABLE;
        canHandle.Init.ABOM = ENABLE;
        canHandle.Init.AWUM = DISABLE;
        canHandle.Init.NART = DISABLE;
        canHandle.Init.RFLM = DISABLE;
        canHandle.Init.TXFP = DISABLE;
        canHandle.Init.Mode = CAN_MODE_NORMAL;
        canHandle.Init.Prescaler = prescaler;
        canHandle.Init.SJW = sjw;
        canHandle.Init.BS1 = bs1;
        canHandle.Init.BS2 = bs2;

        if (HAL_CAN_Init (&canHandle) != HAL_OK) {
                Error_Handler ();
        }

#if 0
        // To pomagało przy monitorAll
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = 0x0000;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;

        if (HAL_CAN_ConfigFilter (&canHandle, &sFilterConfig) != HAL_OK) {
                Error_Handler ();
        }

        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_FMP0);
        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_FMP1);
#endif
}

/**
 * Transmits a sequence of bytes to the ECU over CAN bus
 * @parameter   buff   CanMsgBuffer instance
 * @return the send operation completion status
 */
bool Can::send (const CanFrame &buff, uint32_t timeoutMs)
{
        *canHandle.pTxMsg = buff.toNative ();

        // Sprawdzenie czy można wysłać jest w środku
        if (HAL_CAN_Transmit (&canHandle, timeoutMs) != HAL_OK) {
                return false;
        }

        return true;
}

/**
 * Set the CAN filter for FIFO buffer
 * @parameter   filter    CAN filter value
 * @parameter   mask      CAN mask value
 * @parameter   extended  CAN extended message flag
 * @return  the operation completion status
 */
void Can::setFilterAndMask (uint32_t filter, uint32_t mask, bool extended)
{
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

        if (extended) {
                filter <<= 3;
                mask <<= 3;
                filter |= 0x04;
                mask |= 0x04;

                sFilterConfig.FilterIdHigh = filter >> 16;
                sFilterConfig.FilterIdLow = filter & 0x0000ffff;
                sFilterConfig.FilterMaskIdHigh = mask >> 16;
                sFilterConfig.FilterMaskIdLow = mask & 0x0000ffff;
        }
        else {
                filter <<= 21;
                mask <<= 21;

                sFilterConfig.FilterIdHigh = filter >> 16;
                sFilterConfig.FilterIdLow = filter & 0x0000ffff;
                sFilterConfig.FilterMaskIdHigh = mask >> 16;
                sFilterConfig.FilterMaskIdLow = mask & 0x0000ffff;
        }

        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;

        if (HAL_CAN_ConfigFilter (&canHandle, &sFilterConfig) != HAL_OK) {
                Error_Handler ();
        }
}

/**
 * Read the CAN frame from FIFO buffer
 * @return  true if read the frame / false if no frame
 */
CanFrame Can::read (uint32_t timeoutMs)
{
        if (HAL_CAN_Receive (&canHandle, CAN_FIFO0, timeoutMs) != HAL_OK) {
                return CanFrame ();
        }

        return CanFrame (*canHandle.pRxMsg);
}

/*****************************************************************************/

void Can::clkEnable (CAN_HandleTypeDef *canX)
{
        if (canX->Instance == CAN) {
                __HAL_RCC_CAN1_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void Can::clkDisable (CAN_HandleTypeDef *canX)
{
        if (canX->Instance == CAN) {
                __HAL_RCC_CAN1_CLK_DISABLE ();
        }
}

/*****************************************************************************/

void Can::interrupts (bool on, bool errors)
{
        if (on) {
                if (errors) {
                        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_EWG); // Enable Error warning Interrupt
                        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_EPV); // Enable Error passive Interrupt
                        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_BOF); // Enable Bus-off Interrupt
                        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_LEC); // Enable Last error code Interrupt
                        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_ERR); // Enable Error Interrupt
                }

                __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_FMP0); // Enable FIFO0 interrupt. CAN_IT_FMP1 os also valid value.
        }
        else {
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_EWG);
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_EPV);
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_BOF);
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_LEC);
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_ERR);
                __HAL_CAN_DISABLE_IT (&canHandle, CAN_IT_FMP0);
        }
}

/*****************************************************************************/

void Can::reset ()
{
        // Reset - puts bxCAN controller into sleep mode during which pullup resistors are turned on
        canHandle.Instance->MCR |= CAN_MCR_RESET;
}

/*****************************************************************************/

void Can::disable ()
{
        reset ();
        setMode (INITIALIZATION);
        // Loopback.
        canHandle.Instance->BTR |= CAN_BTR_LBKM;
        interrupts (false);
}

/*****************************************************************************/

void Can::setMode (Mode mode)
{
        switch (mode) {
        case SLEEP:
                canHandle.Instance->MCR &= ~CAN_MCR_INRQ;
                canHandle.Instance->MCR |= CAN_MCR_SLEEP;

                while (!(canHandle.Instance->MSR & CAN_MSR_SLAK))
                        ;

                break;

        case INITIALIZATION:
                canHandle.Instance->MCR &= ~CAN_MCR_SLEEP;
                canHandle.Instance->MCR |= CAN_MCR_INRQ;

                while (!(canHandle.Instance->MSR & CAN_MSR_INAK))
                        ;

                break;

        case NORMAL:
                canHandle.Instance->MCR &= ~(CAN_MCR_SLEEP | CAN_MCR_INRQ);

                while (canHandle.Instance->MSR & CAN_MSR_INAK)
                        ;

                break;
        }
}

/*****************************************************************************/

void Can::setBaudratePrescaler (uint32_t prescaler)
{
        setMode (INITIALIZATION);
        // TODO Działa tylko poniższy sposób, a dwa pod nim nie działają. I nie rozumiem czemu.
        canHandle.Instance->BTR = CAN_MODE_NORMAL | CAN_SJW_1TQ | CAN_BS1_13TQ | CAN_BS2_2TQ | (prescaler - 1);
        // canHandle.Instance->BTR |= ((prescaler - 1) & CAN_BTR_BRP_Msk);
        // volatile uint32_t btr = canHandle.Instance->BTR;
        // canHandle.Instance->BTR = btr | (prescaler - 1);
        setMode (NORMAL);
}

/*****************************************************************************/

void Can::setAutomaticRetransmission (bool b)
{
        if (b) {
                canHandle.Instance->MCR &= ~(1u << 4); // NART
        }
        else {
                canHandle.Instance->MCR |= (1u << 4);
        }
}
