/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Lis3dshAccelerometer.h"
#include "Lis3dshConstants.h"
#include "imu/IAccelerometer.h"

/*****************************************************************************/

Lis3dsh *Lis3dsh::singleton ()
{
        static Lis3dsh l;
        return &l;
}

/*****************************************************************************/

void Lis3dsh::init (ILis3dshBsp *bsp, Lis3dshInit const &initStruct)
{
        this->bsp = bsp;

        /* Configure the low level interface */
        bsp->init ();

        /* Configure MEMS: power mode(ODR) and axes enable */
        uint8_t ctrl = (uint8_t) (initStruct.outputDataRate | initStruct.axesEnable);

        bsp->write (&ctrl, LIS3DSH_CTRL_REG4_ADDR, 1);

        /* Configure MEMS: full scale and self test */
        ctrl = (uint8_t) (initStruct.spiWire | initStruct.selfTest | initStruct.fullScale | initStruct.filterBw);

        bsp->write (&ctrl, LIS3DSH_CTRL_REG5_ADDR, 1);
}

/*****************************************************************************/

void Lis3dsh::reset ()
{
        uint8_t tmpreg;
        bsp->read (&tmpreg, LIS3DSH_CTRL_REG6_ADDR, 1);

        /* Enable or Disable the reboot memory */
        tmpreg |= LIS3DSH_BOOT_FORCED;

        bsp->write (&tmpreg, LIS3DSH_CTRL_REG6_ADDR, 1);
}

/*****************************************************************************/

uint8_t Lis3dsh::getId () const
{
        uint8_t tmp = 0;
        bsp->read (&tmp, LIS3DSH_WHO_AM_I_ADDR, 1);
        return (uint16_t)tmp;
}

/*****************************************************************************/

int8_t Lis3dsh::getTemperature () const
{
        uint8_t tmp = 0;
        bsp->read (&tmp, LIS3DSH_OUT_T_ADDR, 1);
        return tmp + 25;
}

/*****************************************************************************/

bool Lis3dsh::isDataReady () const
{
        if (!bsp) {
                return false;
        }

        uint8_t tmp = 0;
        bsp->read (&tmp, LIS3DSH_STAT_ADDR, 1);
        return tmp & DRDY_BIT_MASK;
}

/*****************************************************************************/

void Lis3dsh::interruptConfig (Lis3dshInterruptConfig const &LIS3DSH_IntConfigStruct)
{

        uint8_t ctrl = 0x00;

        /* Configure Interrupt Selection , Request and Signal */
        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.interruptSelectionEnable | LIS3DSH_IntConfigStruct.interruptRequest
                          | LIS3DSH_IntConfigStruct.interruptSignal);

        bsp->write (&ctrl, LIS3DSH_CTRL_REG3_ADDR, 1);

        /* Configure State Machine 1 */
        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.stateMachine1Enable | LIS3DSH_IntConfigStruct.stateMachine1Interrupt);

        bsp->write (&ctrl, LIS3DSH_CTRL_REG1_ADDR, 1);

        /* Configure State Machine 2 */
        ctrl = (uint8_t) (LIS3DSH_IntConfigStruct.stateMachine2Enable | LIS3DSH_IntConfigStruct.stateMachine2Interrupt);

        bsp->write (&ctrl, LIS3DSH_CTRL_REG2_ADDR, 1);

        bsp->itConfig ();
}

/*****************************************************************************/

void Lis3dsh::clickITConfig ()
{
        uint8_t ctrl = 0x00;
        Lis3dshInterruptConfig lis3dshInterruptStruct;

        /* Set LIS3DSH Interrupt configuration */
        lis3dshInterruptStruct.interruptSelectionEnable = LIS3DSH_INTERRUPT_2_ENABLE;
        lis3dshInterruptStruct.interruptRequest = LIS3DSH_INTERRUPT_REQUEST_LATCHED;
        lis3dshInterruptStruct.interruptSignal = LIS3DSH_INTERRUPT_SIGNAL_HIGH;
        lis3dshInterruptStruct.stateMachine1Enable = LIS3DSH_SM_DISABLE;
        lis3dshInterruptStruct.stateMachine2Enable = LIS3DSH_SM_ENABLE;
        lis3dshInterruptStruct.stateMachine2Interrupt = LIS3DSH_SM_INT1;
        interruptConfig (lis3dshInterruptStruct);

        /* Set LIS3DSH State Machines configuration */
        ctrl = 0x03;
        bsp->write (&ctrl, LIS3DSH_TIM2_1_L_ADDR, 1);
        ctrl = 0xC8;
        bsp->write (&ctrl, LIS3DSH_TIM1_1_L_ADDR, 1);
        ctrl = 0x45;
        bsp->write (&ctrl, LIS3DSH_THRS2_1_ADDR, 1);
        ctrl = 0xFC;
        bsp->write (&ctrl, LIS3DSH_MASK1_A_ADDR, 1);
        ctrl = 0xA1;
        bsp->write (&ctrl, LIS3DSH_SETT1_ADDR, 1);
        ctrl = 0x01;
        bsp->write (&ctrl, LIS3DSH_PR1_ADDR, 1);

        bsp->write (&ctrl, LIS3DSH_SETT2_ADDR, 1);

        /* Configure State Machine 2 to detect single click */
        bsp->write (&ctrl, LIS3DSH_ST2_1_ADDR, 1);
        ctrl = 0x06;
        bsp->write (&ctrl, LIS3DSH_ST2_2_ADDR, 1);
        ctrl = 0x28;
        bsp->write (&ctrl, LIS3DSH_ST2_3_ADDR, 1);
        ctrl = 0x11;
        bsp->write (&ctrl, LIS3DSH_ST2_4_ADDR, 1);
}

/*****************************************************************************/

void Lis3dsh::setDataRate (uint8_t DataRateValue)
{
        uint8_t tmpreg;

        bsp->read (&tmpreg, LIS3DSH_CTRL_REG4_ADDR, 1);

        tmpreg &= (uint8_t)~0xf0;
        tmpreg |= DataRateValue;

        bsp->write (&tmpreg, LIS3DSH_CTRL_REG4_ADDR, 1);
}

/*****************************************************************************/

void Lis3dsh::setFullScale (uint8_t FS_value)
{
        uint8_t tmpreg;

        bsp->read (&tmpreg, LIS3DSH_CTRL_REG5_ADDR, 1);

        /* Set new full scale configuration */
        tmpreg &= (uint8_t)~LIS3DSH_FULLSCALE_16;
        tmpreg |= FS_value;

        bsp->write (&tmpreg, LIS3DSH_CTRL_REG5_ADDR, 1);
}

/*****************************************************************************/

IAccelerometer::AData Lis3dsh::getAData () const
{
        int8_t buffer[6];
        uint8_t crtl;
        float sensitivity = LIS3DSH_SENSITIVITY_0_06G;

        bsp->read (&crtl, LIS3DSH_CTRL_REG5_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[0], LIS3DSH_OUT_X_L_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[1], LIS3DSH_OUT_X_H_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[2], LIS3DSH_OUT_Y_L_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[3], LIS3DSH_OUT_Y_H_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[4], LIS3DSH_OUT_Z_L_ADDR, 1);
        bsp->read ((uint8_t *)&buffer[5], LIS3DSH_OUT_Z_H_ADDR, 1);

        switch (crtl & LIS3DSH__FULLSCALE_SELECTION) {
        /* FS bit = 000 ==> Sensitivity typical value = 0.06milligals/digit */
        case LIS3DSH_FULLSCALE_2:
                sensitivity = LIS3DSH_SENSITIVITY_0_06G;
                break;

        /* FS bit = 001 ==> Sensitivity typical value = 0.12milligals/digit */
        case LIS3DSH_FULLSCALE_4:
                sensitivity = LIS3DSH_SENSITIVITY_0_12G;
                break;

        /* FS bit = 010 ==> Sensitivity typical value = 0.18milligals/digit */
        case LIS3DSH_FULLSCALE_6:
                sensitivity = LIS3DSH_SENSITIVITY_0_18G;
                break;

        /* FS bit = 011 ==> Sensitivity typical value = 0.24milligals/digit */
        case LIS3DSH_FULLSCALE_8:
                sensitivity = LIS3DSH_SENSITIVITY_0_24G;
                break;

        /* FS bit = 100 ==> Sensitivity typical value = 0.73milligals/digit */
        case LIS3DSH_FULLSCALE_16:
                sensitivity = LIS3DSH_SENSITIVITY_0_73G;
                break;

        default:
                break;
        }

        AData data;
        data.x = (int16_t) (((buffer[1] << 8) + buffer[0]) * sensitivity + 0.5);
        data.y = (int16_t) (((buffer[3] << 8) + buffer[2]) * sensitivity + 0.5);
        data.z = (int16_t) (((buffer[5] << 8) + buffer[4]) * sensitivity + 0.5);
        return data;
}
