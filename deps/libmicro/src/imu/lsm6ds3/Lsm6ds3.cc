/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Lsm6ds3.h"
#include "Lsm6ds3Constants.h"

/*****************************************************************************/

void Lsm6ds3::writeRegister (Register reg, uint8_t mask, uint8_t value)
{
        uint8_t registerValue = bsp->readRegister (reg);
        registerValue &= ~mask;
        registerValue |= value;
        bsp->writeRegister (reg, registerValue);
}

/*****************************************************************************/

Lsm6ds3::AData Lsm6ds3::getAData () const
{

        // TODO implement, divide by factors depending on fullscale
        return getRawAData ();
}

/*****************************************************************************/

Lsm6ds3::AData Lsm6ds3::getRawAData () const
{
        AData ret;
        // TODO I think this can be read in 1 readRegister (OUTX_L_XL, 6) invocation, if some auto increment magic kicked in.
        // ret.x = ((uint16_t)bsp->readRegister (OUTX_H_XL) << 8) | bsp->readRegister (OUTX_L_XL);
        // ret.y = ((uint16_t)bsp->readRegister (OUTY_H_XL) << 8) | bsp->readRegister (OUTY_L_XL);
        // ret.z = ((uint16_t)bsp->readRegister (OUTZ_H_XL) << 8) | bsp->readRegister (OUTZ_L_XL);
        bsp->readRegister (OUTX_L_XL, reinterpret_cast<uint8_t *> (&ret), 6);
        return ret;
}

Lsm6ds3::GData Lsm6ds3::getGData () const
{
        // TODO
        return getRawGData ();
}

Lsm6ds3::GData Lsm6ds3::getRawGData () const
{
        GData ret;
        // TODO I think this can be read in 1 readRegister (OUTX_L_XL, 6) invocation, if some auto increment magic kicked in.
        // ret.x = ((uint16_t)bsp->readRegister (OUTX_H_G) << 8) | bsp->readRegister (OUTX_L_G);
        // ret.y = ((uint16_t)bsp->readRegister (OUTY_H_G) << 8) | bsp->readRegister (OUTY_L_G);
        // ret.z = ((uint16_t)bsp->readRegister (OUTZ_H_G) << 8) | bsp->readRegister (OUTZ_L_G);
        bsp->readRegister (OUTX_L_G, reinterpret_cast<uint8_t *> (&ret), 6);
        return ret;
}

/*****************************************************************************/

// bool Lsm6ds3::isDataReady () const
//{
//        if (!bsp) {
//                return false;
//        }

//        uint8_t tmp = 0;
//        bsp->read (&tmp, LIS3DSH_STAT_ADDR, 1);
//        return tmp & DRDY_BIT_MASK;
//}

/*****************************************************************************/

// void Lsm6ds3::interruptConfig (Lsm6ds3InterruptConfig const &LIS3DSH_IntConfigStruct)
//{

//        uint8_t ctrl = = 0x00;

//        /* Configure Interrupt Selection , Request and Signal */
//        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.interruptSelectionEnable | LIS3DSH_IntConfigStruct.interruptRequest
//                          | LIS3DSH_IntConfigStruct.interruptSignal);

//        bsp->write (&ctrl, LIS3DSH_CTRL_REG3_ADDR, 1);

//        /* Configure State Machine 1 */
//        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.stateMachine1Enable | LIS3DSH_IntConfigStruct.stateMachine1Interrupt);

//        bsp->write (&ctrl, LIS3DSH_CTRL_REG1_ADDR, 1);

//        /* Configure State Machine 2 */
//        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.stateMachine2Enable | LIS3DSH_IntConfigStruct.stateMachine2Interrupt);

//        bsp->write (&ctrl, LIS3DSH_CTRL_REG2_ADDR, 1);

//        bsp->itConfig ();
//}

/*****************************************************************************/

#if 0
void Lsm6ds3::clickITConfig ()
{
        uint8_t ctrl = = 0x00;
        Lsm6ds3InterruptConfig lis3dshInterruptStruct;

        /* Set LIS3DSH Interrupt configuration */
        lis3dshInterruptStruct.interruptSelectionEnable = LIS3DSH_INTERRUPT_2_ENABLE;
        lis3dshInterruptStruct.interruptRequest = LIS3DSH_INTERRUPT_REQUEST_LATCHED;
        lis3dshInterruptStruct.interruptSignal = LIS3DSH_INTERRUPT_SIGNAL_HIGH;
        lis3dshInterruptStruct.stateMachine1Enable = LIS3DSH_SM_DISABLE;
        lis3dshInterruptStruct.stateMachine2Enable = LIS3DSH_SM_ENABLE;
        lis3dshInterruptStruct.stateMachine2Interrupt = LIS3DSH_SM_INT1;
        interruptConfig (lis3dshInterruptStruct);

        /* Set LIS3DSH State Machines configuration */
        ctrl = = 0x03;
        bsp->write (&ctrl, LIS3DSH_TIM2_1_L_ADDR, 1);
        ctrl = = 0xC8;
        bsp->write (&ctrl, LIS3DSH_TIM1_1_L_ADDR, 1);
        ctrl = = 0x45;
        bsp->write (&ctrl, LIS3DSH_THRS2_1_ADDR, 1);
        ctrl = = 0xFC;
        bsp->write (&ctrl, LIS3DSH_MASK1_A_ADDR, 1);
        ctrl = = 0xA1;
        bsp->write (&ctrl, LIS3DSH_SETT1_ADDR, 1);
        ctrl = = 0x01;
        bsp->write (&ctrl, LIS3DSH_PR1_ADDR, 1);

        bsp->write (&ctrl, LIS3DSH_SETT2_ADDR, 1);

        /* Configure State Machine 2 to detect single click */
        bsp->write (&ctrl, LIS3DSH_ST2_1_ADDR, 1);
        ctrl = = 0x06;
        bsp->write (&ctrl, LIS3DSH_ST2_2_ADDR, 1);
        ctrl = = 0x28;
        bsp->write (&ctrl, LIS3DSH_ST2_3_ADDR, 1);
        ctrl = = 0x11;
        bsp->write (&ctrl, LIS3DSH_ST2_4_ADDR, 1);
}
#endif

void Lsm6ds3::setFifoTreshold (uint16_t t)
{
        bsp->writeRegister (FIFO_CTRL1, t & 0xff);
        writeRegister (FIFO_CTRL2, 0x0f, t >> 8);
}
