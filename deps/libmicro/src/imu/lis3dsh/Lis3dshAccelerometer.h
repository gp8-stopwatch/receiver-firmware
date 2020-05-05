/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ACCELERO_LIS3DSH_H
#define ACCELERO_LIS3DSH_H

#include "ILis3dshBsp.h"
#include "imu/IAccelerometer.h"
#include <cstdint>

/// Power mode, frequency
enum DataRatePowerMode {
        LIS3DSH_DATARATE_POWERDOWN = 0x00,
        LIS3DSH_DATARATE_3_125 = 0x10, // 3.125 Hz
        LIS3DSH_DATARATE_6_25 = 0x20,  // 6.25 Hz
        LIS3DSH_DATARATE_12_5 = 0x30,  // 12.5 Hz
        LIS3DSH_DATARATE_25 = 0x40,    // 25 Hz
        LIS3DSH_DATARATE_50 = 0x50,    // 50 Hz
        LIS3DSH_DATARATE_100 = 0x60,   // 100 Hz
        LIS3DSH_DATARATE_400 = 0x70,   // 400 Hz
        LIS3DSH_DATARATE_800 = 0x80,   // 800 Hz
        LIS3DSH_DATARATE_1600 = 0x90   // 1600 Hz
};

enum FullScale {
        LIS3DSH_FULLSCALE_2 = 0x00, /* 2 g  */
        LIS3DSH_FULLSCALE_4 = 0x08, /* 4 g  */
        LIS3DSH_FULLSCALE_6 = 0x10, /* 6 g  */
        LIS3DSH_FULLSCALE_8 = 0x18, /* 8 g  */
        LIS3DSH_FULLSCALE_16 = 0x20 /* 16 g */
};

enum AntiAliasingFilterBandwidth {
        LIS3DSH_FILTER_BW_800 = 0x00, /* 800 Hz */
        LIS3DSH_FILTER_BW_40 = 0x08,  /* 40 Hz  */
        LIS3DSH_FILTER_BW_200 = 0x10, /* 200 Hz */
        LIS3DSH_FILTER_BW_50 = 0x18   /* 50 Hz  */
};

enum SelfTest { LIS3DSH_SELFTEST_NORMAL = 0x00, LIS3DSH_SELFTEST_P = 0x02, LIS3DSH_SELFTEST_M = 0x04 };
enum DirectionXYZ { LIS3DSH_X_ENABLE = 0x01, LIS3DSH_Y_ENABLE = 0x02, LIS3DSH_Z_ENABLE = 0x04, LIS3DSH_XYZ_ENABLE = 0x07 };
enum SpiSerialInterfaceMode { LIS3DSH_SERIALINTERFACE_4WIRE = 0x00, LIS3DSH_SERIALINTERFACE_3WIRE = 0x01 };

/**
 * Służy do inicjowania
 */
struct Lis3dshInit {
        DataRatePowerMode outputDataRate;     /* Output Data Rate into CTRL_REG4 register */
        uint8_t axesEnable;                   /* Axes enable into CTRL_REG4 register */
        SpiSerialInterfaceMode spiWire;       /* SPI Wire into CTRL_REG5 register */
        uint8_t selfTest;                     /* Self test into CTRL_REG5 register */
        FullScale fullScale;                  /* Full scale into CTRL_REG5 register */
        AntiAliasingFilterBandwidth filterBw; /* Filter BW into CTRL_REG5 register */
};

enum InterruptRequest { LIS3DSH_INTERRUPT_REQUEST_LATCHED = 0x00, LIS3DSH_INTERRUPT_REQUEST_PULSED = 0x20 };
enum InterruptSelectionEnable { LIS3DSH_INTERRUPT_1_ENABLE = 0x88, LIS3DSH_INTERRUPT_2_ENABLE = 0x10, LIS3DSH_INTERRUPT_1_2_ENABLE = 0x98 };
enum InterruptSignalActive { LIS3DSH_INTERRUPT_SIGNAL_LOW = 0x00, LIS3DSH_INTERRUPT_SIGNAL_HIGH = 0x40 };
enum StateMachineEnable { LIS3DSH_SM_DISABLE = 0x00, LIS3DSH_SM_ENABLE = 0x01 };
enum StateMachineRoutedInterrupt { LIS3DSH_SM_INT1 = 0x00, LIS3DSH_SM_INT2 = 0x08 };

/**
 * Do konfigurowania przerwań.
 */
struct Lis3dshInterruptConfig {
        InterruptRequest interruptRequest;                  /* Latch/Pulse Interrupt request into CTRL_REG3 register */
        InterruptSelectionEnable interruptSelectionEnable;  /* Interrupt1/2 selection enable into CTRL_REG3 register */
        InterruptSignalActive interruptSignal;              /* Interrupt signal active Low/High into CTRL_REG3 register */
        StateMachineEnable stateMachine1Enable;             /* State Machine 1 enable/disable into CTRL_REG1 register */
        StateMachineRoutedInterrupt stateMachine1Interrupt; /* State Machine 1 routed Interrupt into CTRL_REG1 register */
        StateMachineEnable stateMachine2Enable;             /* State Machine 2 enable/disable into CTRL_REG2 register */
        StateMachineRoutedInterrupt stateMachine2Interrupt; /* State Machine 2 routed Interrupt into CTRL_REG1 register */
};

/**
 * LIS3DSH API the way I like it.
 */
class Lis3dsh : public IAccelerometer {
public:
        Lis3dsh () : bsp (nullptr) {}
        virtual ~Lis3dsh () override = default;
        static Lis3dsh *singleton ();

        void init (ILis3dshBsp *bsp, Lis3dshInit const &initStruct);
        void reset ();

        uint8_t getId () const;
        AData getAData () const override;

        int8_t getTemperature () const;
        /// Checks for interrupt status DRDY
        bool isDataReady () const;

        // Config
        void interruptConfig (Lis3dshInterruptConfig const &LIS3DSH_IntConfigStruct);
        void clickITConfig ();
        void setDataRate (uint8_t DataRateValue);
        void setFullScale (uint8_t FS_value);

private:
        ILis3dshBsp *bsp;
};

#endif // LIS3DSH_H
