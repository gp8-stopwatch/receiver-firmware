/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ACCELERO_LSM6ds3_H
#define ACCELERO_LSM6ds3_H

#include "imu/IAccelerometer.h"
#include "imu/IGyroscope.h"
#include "imu/lsm6ds3/ILsm6ds3Bsp.h"
#include <cstdint>

/**
 * Lsm6ds3 API the way I like it.
 */
class Lsm6ds3 : public IAccelerometer, public IGyroscope {
public:
        Lsm6ds3 (ILsm6ds3Bsp *b) : bsp (b) {}
        virtual ~Lsm6ds3 () {}

        /*---------------------------------------------------------------------------*/

        AData getAData () const;
        AData getRawAData () const;
        GData getGData () const;
        GData getRawGData () const;

        // TODO hide.
        uint8_t readRegister (uint8_t r) { return bsp->readRegister (r); }

        /*****************************************************************************/
        /*                                                                           */
        /*****************************************************************************/

        enum { WHO_AM_I_ADDRESS = 0x69 };
        /// Should return 0x69
        uint8_t getWhoAmI () const { return bsp->readRegister (WHO_AM_I_REG); }

        /*****************************************************************************/
        /* CTRL3_C                                                                   */
        /*****************************************************************************/

        enum { BOOT = 0x80, BOOT_MASK = 0x80 };
        void rebootMemory () { writeRegister (CTRL3_C, BOOT_MASK, BOOT); }

        /// Block Data Update
        enum Bdu { CONTINUOS = 0x00, BLOCK_UPDATE = 0x40, BDU_MASK = 0x40 };
        Bdu getBdu () const { return static_cast<Bdu> (bsp->readRegister (CTRL3_C) & BDU_MASK); }
        void setBdu (Bdu b) { writeRegister (CTRL3_C, BDU_MASK, b); }

        enum { SW_RESET = 0x01, SW_RESET_MASK = 0x01 };
        void softwareReset () { writeRegister (CTRL3_C, SW_RESET_MASK, SW_RESET); }

        /*****************************************************************************/
        /*                                                                           */
        /*****************************************************************************/

        enum AccelFullScale { FS_2G = 0x00, FS_16G = 0x04, FS_4G = 0x08, FS_8G = 0x0C, ACCEL_FS_MASK = 0x0c };

        AccelFullScale getAccelFullScale () const { return static_cast<AccelFullScale> (bsp->readRegister (CTRL1_XL) & ACCEL_FS_MASK); }
        void setAccelFullScale (AccelFullScale a) { writeRegister (CTRL1_XL, ACCEL_FS_MASK, a); }

        /*---------------------------------------------------------------------------*/

        enum AccelOdr {
                ACCEL_ODR_POWER_DOWN = 0x00,
                ACCEL_ODR_13Hz = 0x10,
                ACCEL_ODR_26Hz = 0x20,
                ACCEL_ODR_52Hz = 0x30,
                ACCEL_ODR_104Hz = 0x40,
                ACCEL_ODR_208Hz = 0x50,
                ACCEL_ODR_416Hz = 0x60,
                ACCEL_ODR_833Hz = 0x70,
                ACCEL_ODR_1660Hz = 0x80,
                ACCEL_ODR_3330Hz = 0x90,
                ACCEL_ODR_6660Hz = 0xA0,
                ACCEL_ODR_MASK = 0xF0
        };

        AccelOdr getAccelOdr () const { return static_cast<AccelOdr> (bsp->readRegister (CTRL1_XL) & ACCEL_ODR_MASK); }
        void setAccelOdr (AccelOdr b) { writeRegister (CTRL1_XL, ACCEL_ODR_MASK, b); }

        /*---------------------------------------------------------------------------*/

        enum AntiAliasingBandwidth { BW_400Hz = 0x00, BW_200Hz = 0x01, BW_100Hz = 0x02, BW_50Hz = 0x03, BW_MASK = 0x03 };
        AntiAliasingBandwidth getAntiAliasingBandwidth () const
        {
                return static_cast<AntiAliasingBandwidth> (bsp->readRegister (CTRL1_XL) & BW_MASK);
        }

        void setAntiAliasingBandwidth (AntiAliasingBandwidth b) { writeRegister (CTRL1_XL, BW_MASK, b); }

        /*****************************************************************************/
        /*                                                                           */
        /*****************************************************************************/

        enum GyroFullScale { FS_245dps = 0x00, FS_500dps = 0x04, FS_1000dps = 0x08, FS_2000dps = 0x0C, GYRO_FS_MASK = 0x0c };

        GyroFullScale getGyroFullScale () const { return static_cast<GyroFullScale> (bsp->readRegister (CTRL2_G) & GYRO_FS_MASK); }
        void setGyroFullScale (GyroFullScale a) { writeRegister (CTRL2_G, GYRO_FS_MASK, a); }

        /*---------------------------------------------------------------------------*/

        enum GyroOdr {
                GYRO_ODR_POWER_DOWN = 0x00,
                GYRO_ODR_13Hz = 0x10,
                GYRO_ODR_26Hz = 0x20,
                GYRO_ODR_52Hz = 0x30,
                GYRO_ODR_104Hz = 0x40,
                GYRO_ODR_208Hz = 0x50,
                GYRO_ODR_416Hz = 0x60,
                GYRO_ODR_833Hz = 0x70,
                GYRO_ODR_1660Hz = 0x80,
                GYRO_ODR_MASK = 0xF0
        };

        GyroOdr getGyroOdr () const { return static_cast<GyroOdr> (bsp->readRegister (CTRL2_G) & GYRO_ODR_MASK); }
        void setGyroOdr (GyroOdr b) { writeRegister (CTRL2_G, GYRO_ODR_MASK, b); }

        /*---------------------------------------------------------------------------*/

        /// Gyroscope full-scale at 125 dps. Default value: 0
        enum FS_125_DPS { FS_125_DISABLED = 0x00, FS_125_ENABLED = 0x02, FS_125_MASK = 0x02 };
        bool isFs125Dps () const { return static_cast<GyroOdr> (bsp->readRegister (CTRL2_G) & FS_125_MASK); }
        void setFs125Dps (bool b) { writeRegister (CTRL2_G, FS_125_MASK, (b) ? (FS_125_ENABLED) : (FS_125_DISABLED)); }

        /*****************************************************************************/
        /* CTRL4_C                                                                   */
        /*****************************************************************************/

        enum I2cOperation { I2C_AND_SPI = 0x00, SPI_ONLY = 0x04, I2C_DISABLE_MASK = 0x04 };
        bool isI2cEnable () const { return bsp->readRegister (CTRL4_C) & I2C_DISABLE_MASK; }
        void setI2cEnable (bool b) { writeRegister (CTRL4_C, I2C_DISABLE_MASK, (b) ? (I2C_AND_SPI) : (SPI_ONLY)); };

        /// Enable FIFO threshold level use. Limit FIFO size to the treshold.
        bool isFifoStopOnFth () const { return bsp->readRegister (CTRL4_C) & 0x01; }
        void setFifoStopOnFth (bool b) { writeRegister (CTRL4_C, 0x01, (b) ? (0x01) : (0x00)); };

        /*****************************************************************************/
        /* FIFO                                                                      */
        /*****************************************************************************/

        uint16_t getFifoTreshold () const { return bsp->readRegister (FIFO_CTRL1) | uint16_t (bsp->readRegister (FIFO_CTRL2) & 0x0f) << 8; }
        /// I think, that units are 2B.
        void setFifoTreshold (uint16_t t);

        bool isFifoTimerPedoEnabled () const { return bsp->readRegister (FIFO_CTRL2) & 0x80; }
        /// Enables step counter and timestamp data to be stored as the 4 th FIFO data set.
        void setFifoTimerPedoEnabled (bool b) { writeRegister (FIFO_CTRL2, 0x80, (b) ? (0x80) : (0)); }

        bool isFifoTimerPedoDrdy () const { return bsp->readRegister (FIFO_CTRL2) & 0x80; }
        void setFifoTimerPedoDrdy (bool b) { writeRegister (FIFO_CTRL2, 0x80, (b) ? (0x80) : (0)); }

        enum FifoAccelDecimation {
                ACCEL_DATA_NOT_IN_FIFO = 0x00,
                ACCEL_NO_DECIMATION = 0x01,
                ACCEL_DECIMATION_BY_2 = 0x02,
                ACCEL_DECIMATION_BY_3 = 0x03,
                ACCEL_DECIMATION_BY_4 = 0x04,
                ACCEL_DECIMATION_BY_8 = 0x05,
                ACCEL_DECIMATION_BY_16 = 0x06,
                ACCEL_DECIMATION_BY_32 = 0x07,
                ACCEL_FIFO_DECIMATION_MASK = 0x07
        };

        FifoAccelDecimation getFifoAccelDecimation () const
        {
                return static_cast<FifoAccelDecimation> (bsp->readRegister (FIFO_CTRL3) & ACCEL_FIFO_DECIMATION_MASK);
        }
        void setFifoAccelDecimation (FifoAccelDecimation b) { writeRegister (FIFO_CTRL3, ACCEL_FIFO_DECIMATION_MASK, b); }

        enum FifoGyroDecimation {
                GYRO_DATA_NOT_IN_FIFO = 0x00,
                GYRO_NO_DECIMATION = 0x08,
                GYRO_DECIMATION_BY_2 = 0x10,
                GYRO_DECIMATION_BY_3 = 0x18,
                GYRO_DECIMATION_BY_4 = 0x20,
                GYRO_DECIMATION_BY_8 = 0x28,
                GYRO_DECIMATION_BY_16 = 0x30,
                GYRO_DECIMATION_BY_32 = 0x38,
                GYRO_FIFO_DECIMATION_MASK = 0x38
        };

        FifoGyroDecimation getFifoGyroDecimation () const
        {
                return static_cast<FifoGyroDecimation> (bsp->readRegister (FIFO_CTRL3) & GYRO_FIFO_DECIMATION_MASK);
        }
        void setFifoGyroDecimation (FifoGyroDecimation b) { writeRegister (FIFO_CTRL3, GYRO_FIFO_DECIMATION_MASK, b); }

        bool isFifoOnlyHighData () const { return bsp->readRegister (FIFO_CTRL4) & 0x40; }
        void setFifoOnlyHighData (bool b) { writeRegister (CTRL4_C, 0x40, (b) ? (0x40) : (0)); };

        enum FifoMode {
                FIFO_MODE_BYPASS = 0x00,
                FIFO_MODE_FIFO = 0x01,
                FIFO_MODE_CONTINUOUS_TO_FIFO = 0x03,
                FIFO_MODE_BYPASS_TO_CONTINUOUS = 0x04,
                FIFO_MODE_CONTINUOUS = 0x06,
                FIFO_MODE_MASK = 0x07
        };

        FifoMode getFifoMode () const { return static_cast<FifoMode> (bsp->readRegister (FIFO_CTRL5) & FIFO_MODE_MASK); }
        void setFifoMode (FifoMode b) { writeRegister (FIFO_CTRL5, FIFO_MODE_MASK, b); }

        enum FifoOdr {
                FIFO_ODR_DISABLED = 0x00,
                FIFO_ODR_13Hz = 0x08,
                FIFO_ODR_26Hz = 0x10,
                FIFO_ODR_52Hz = 0x18,
                FIFO_ODR_104Hz = 0x20,
                FIFO_ODR_208Hz = 0x28,
                FIFO_ODR_416Hz = 0x30,
                FIFO_ODR_833Hz = 0x38,
                FIFO_ODR_1660Hz = 0x40,
                FIFO_ODR_3330Hz = 0x48,
                FIFO_ODR_6660Hz = 0x50,
                FIFO_ODR_MASK = 0x78
        };

        FifoOdr getFifoOdr () const { return static_cast<FifoOdr> (bsp->readRegister (FIFO_CTRL5) & FIFO_ODR_MASK); }
        void setFifoOdr (FifoOdr b) { writeRegister (FIFO_CTRL5, FIFO_ODR_MASK, b); }

        /// Number of samples already stored in FIFO.
        uint16_t getFifoSamplesNum () const
        {
                return bsp->readRegister (FIFO_STATUS1) | uint16_t (bsp->readRegister (FIFO_STATUS2) & 0x0f) << 8;
        }

        /// This bit is set high when the number of bytes already stored in the FIFO is equal to or higher than the watermark level
        bool isFifoWatermarkReached () const { return bsp->readRegister (FIFO_STATUS2) & 0x80; }

        /// High when the FIFO is completely filled and at least one sample has already been overwritten to store the new data.
        bool isFifoOverrun () const { return bsp->readRegister (FIFO_STATUS2) & 0x40; }
        bool isFifoFull () const { return bsp->readRegister (FIFO_STATUS2) & 0x20; }
        bool isFifoEmpty () const { return bsp->readRegister (FIFO_STATUS2) & 0x10; }

        /// Which axis of which sensor data will be read at the next reading.
        uint16_t getFifoPattern () const { return bsp->readRegister (FIFO_STATUS3) | uint16_t (bsp->readRegister (FIFO_STATUS4) & 0x03) << 8; }

        int16_t getFifoNextSample () const
        {
                return (uint16_t)bsp->readRegister (FIFO_DATA_OUT_H) << 8 | (uint16_t)bsp->readRegister (FIFO_DATA_OUT_L);
        }

        /*****************************************************************************/
        /* Interrupt configuration                                                   */
        /*****************************************************************************/

        enum Interrupt1Source {
                INT1_STEP_DETECTOR = 1 << 7, /// Pedometer step recognition interrupt enable on INT1 pad.
                INT1_SIGN_MOT = 1 << 6,      /// Significant motion interrupt enable on INT1 pad.
                INT1_FULL_FLAG = 1 << 5,     /// FIFO full flag interrupt enable on INT1 pad.
                INT1_FIFO_OVR = 1 << 4,      /// FIFO overrun interrupt on INT1 pad.
                INT1_FTH = 1 << 3,           /// FIFO threshold interrupt on INT1 pad.
                INT1_BOOT = 1 << 2,          /// Boot status available on INT1 pad.
                INT1_DRDY_G = 1 << 1,        /// Gyroscope Data Ready on INT1 pad.
                INT1_DRDY_XL = 1 << 0        /// Accelerometer Data Ready on INT1 pad.
        };

        void setInt1Source (uint8_t s) { bsp->writeRegister (INT1_CTRL, s); }
        void addInt1Source (Interrupt1Source s) { writeRegister (INT1_CTRL, s, s); }
        void removeInt1Source (Interrupt1Source s) { writeRegister (INT1_CTRL, s, 0); }
        uint8_t getInt1Source () const;

        /*****************************************************************************/
        /* Miscellaneous                                                             */
        /*****************************************************************************/

        int16_t getTemperature () const
        {
                return (int16_t) ((uint16_t)bsp->readRegister (OUT_TEMP_H) << 8 | (uint16_t)bsp->readRegister (OUT_TEMP_L)) / 16 + 25;
        }

private:
        enum Register {
                FUNC_CFG_ACCESS = 0x01,
                SENSOR_SYNC_TIME = 0x04,
                FIFO_CTRL1 = 0x06,
                FIFO_CTRL2 = 0x07,
                FIFO_CTRL3 = 0x08,
                FIFO_CTRL4 = 0x09,
                FIFO_CTRL5 = 0x0A,
                ORIENT_CFG_G = 0x0B,
                INT1_CTRL = 0x0D,
                INT2_CTRL = 0x0E,
                WHO_AM_I_REG = 0x0F,
                CTRL1_XL = 0x10,
                CTRL2_G = 0x11,
                CTRL3_C = 0x12,
                CTRL4_C = 0x13,
                CTRL5_C = 0x14,
                CTRL6_G = 0x15,
                CTRL7_G = 0x16,
                CTRL8_XL = 0x17,
                CTRL9_XL = 0x18,
                CTRL10_C = 0x19,
                MASTER_CONFIG = 0x1A,
                WAKE_UP_SRC = 0x1B,
                TAP_SRC = 0x1C,
                D6D_SRC = 0x1D,
                STATUS_REG = 0x1E,
                OUT_TEMP_L = 0x20,
                OUT_TEMP_H = 0x21,
                OUTX_L_G = 0x22,
                OUTX_H_G = 0x23,
                OUTY_L_G = 0x24,
                OUTY_H_G = 0x25,
                OUTZ_L_G = 0x26,
                OUTZ_H_G = 0x27,
                OUTX_L_XL = 0x28,
                OUTX_H_XL = 0x29,
                OUTY_L_XL = 0x2A,
                OUTY_H_XL = 0x2B,
                OUTZ_L_XL = 0x2C,
                OUTZ_H_XL = 0x2D,
                SENSORHUB1_REG = 0x2E,
                SENSORHUB2_REG = 0x2F,
                SENSORHUB3_REG = 0x30,
                SENSORHUB4_REG = 0x31,
                SENSORHUB5_REG = 0x32,
                SENSORHUB6_REG = 0x33,
                SENSORHUB7_REG = 0x34,
                SENSORHUB8_REG = 0x35,
                SENSORHUB9_REG = 0x36,
                SENSORHUB10_REG = 0x37,
                SENSORHUB11_REG = 0x38,
                SENSORHUB12_REG = 0x39,
                FIFO_STATUS1 = 0x3A,
                FIFO_STATUS2 = 0x3B,
                FIFO_STATUS3 = 0x3C,
                FIFO_STATUS4 = 0x3D,
                FIFO_DATA_OUT_L = 0x3E,
                FIFO_DATA_OUT_H = 0x3F,
                TIMESTAMP0_REG = 0x40,
                TIMESTAMP1_REG = 0x41,
                TIMESTAMP2_REG = 0x42,
                TIMESTAMP_L = 0x49,
                TIMESTAMP_H = 0x4A,
                STEP_COUNTER_L = 0x4B,
                STEP_COUNTER_H = 0x4C,
                SENSORHUB13_REG = 0x4D,
                SENSORHUB14_REG = 0x4E,
                SENSORHUB15_REG = 0x4F,
                SENSORHUB16_REG = 0x50,
                SENSORHUB17_REG = 0x51,
                SENSORHUB18_REG = 0x52,
                FUNC_SRC = 0x53,
                TAP_CFG1 = 0x58,
                TAP_THS_6D = 0x59,
                INT_DUR2 = 0x5A,
                WAKE_UP_THS = 0x5B,
                WAKE_UP_DUR = 0x5C,
                FREE_FALL = 0x5D,
                MD1_CFG = 0x5E,
                MD2_CFG = 0x5F
        };

        /**
         * Clears the mask, writes value.
         */
        void writeRegister (Register reg, uint8_t mask, uint8_t value);
        void writeRegister (Register reg, uint8_t value) { bsp->writeRegister (reg, value); }

        enum FunctionRegister {
                SLV0_ADD = 0x02,
                SLV0_SUBADD = 0x03,
                SLAVE0_CONFIG = 0x04,
                SLV1_ADD = 0x05,
                SLV1_SUBADD = 0x06,
                SLAVE1_CONFIG = 0x07,
                SLV2_ADD = 0x08,
                SLV2_SUBADD = 0x09,
                SLAVE2_CONFIG = 0x0A,
                SLV3_ADD = 0x0B,
                SLV3_SUBADD = 0x0C,
                SLAVE3_CONFIG = 0x0D,
                DATAWRITE_SRC_MODE_SUB_SLV0 = 0x0E,
                CONFIG_PEDO_THS_MIN = 0x0F,
                SM_STEP_THS = 0x13,
                PEDO_DEB_REG = 0x14,
                STEP_COUNT_DELTA = 0x15,
                MAG_SI_XX = 0x24,
                MAG_SI_XY = 0x25,
                MAG_SI_XZ = 0x26,
                MAG_SI_YX = 0x27,
                MAG_SI_YY = 0x28,
                MAG_SI_YZ = 0x29,
                MAG_SI_ZX = 0x2A,
                MAG_SI_ZY = 0x2B,
                MAG_SI_ZZ = 0x2C,
                MAG_OFFX_L = 0x2D,
                MAG_OFFX_H = 0x2E,
                MAG_OFFY_L = 0x2F,
                MAG_OFFY_H = 0x30,
                MAG_OFFZ_L = 0x31,
                MAG_OFFZ_H = 0x32
        };

private:
        ILsm6ds3Bsp *bsp;
};

#endif // LIS3DSH_H
