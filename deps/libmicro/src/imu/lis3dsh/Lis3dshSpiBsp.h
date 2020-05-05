/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIS3DSHSPIBSP_H
#define LIS3DSHSPIBSP_H

#include "Hal.h"
#include "ILis3dshBsp.h"

/// Read/Write command
#define READWRITE_CMD ((uint8_t)0x80)
/// Multiple byte read/write command
#define MULTIPLEBYTE_CMD ((uint8_t)0x40)

/// Chip Select macro definition
#define ACCELERO_CS_LOW() HAL_GPIO_WritePin (ACCELERO_CS_GPIO_PORT, ACCELERO_CS_PIN, GPIO_PIN_RESET)
#define ACCELERO_CS_HIGH() HAL_GPIO_WritePin (ACCELERO_CS_GPIO_PORT, ACCELERO_CS_PIN, GPIO_PIN_SET)

/*****************************************************************************/

#define ACCELERO_CS_PIN GPIO_PIN_12
#define ACCELERO_CS_GPIO_PORT GPIOB
#define ACCELERO_CS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE ()
#define ACCELERO_CS_GPIO_CLK_DISABLE() __GPIOE_CLK_DISABLE ()

#define ACCELERO_INT_GPIO_PORT GPIOB
#define ACCELERO_INT_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE ()
#define ACCELERO_INT_GPIO_CLK_DISABLE() __GPIOB_CLK_DISABLE ()
#define ACCELERO_INT1_PIN GPIO_PIN_2
#define ACCELERO_INT1_EXTI_IRQn EXTI2_3_IRQn
#define ACCELERO_INT2_PIN GPIO_PIN_1
#define ACCELERO_INT2_EXTI_IRQn EXTI0_1_IRQn
#define ACCELERO_INT1_HANDLER EXTI2_3_IRQHandler
#define ACCELERO_INT2_HANDLER EXTI0_1_IRQHandler

extern "C" void ACCELERO_INT1_HANDLER ();
extern "C" void ACCELERO_INT2_HANDLER ();

/**
 * Interfejs dla klas chcących obsłurzyć przerwania akcleerometru. Te funkcje wykonają się w ISR.
 */
struct ILis3dshSpiBspListener {
        virtual ~ILis3dshSpiBspListener () {}
        virtual void onInterrupt1 () = 0;
        virtual void onInterrupt2 () = 0;
};

/**
 * Low level implementation. Reimplement only this one class when moving to another
 * board / µC.
 */
class Lis3dshSpiBsp : public ILis3dshBsp {
public:
        virtual ~Lis3dshSpiBsp () {}
        static Lis3dshSpiBsp *singleton ();

        void init ();
        void itConfig ();
        void write (uint8_t *buffer, uint8_t addr, uint16_t len);
        void read (uint8_t *buffer, uint8_t addr, uint16_t len);

        void setLis3dshBspListener (ILis3dshSpiBspListener *l) { lis3dshBspListener = l; }

private:
        Lis3dshSpiBsp () : lis3dshBspListener (nullptr) {}
        friend void ACCELERO_INT1_HANDLER ();
        friend void ACCELERO_INT2_HANDLER ();

        uint8_t spiWriteRead (uint8_t byte);
        SPI_HandleTypeDef accelSpiHandle;
        ILis3dshSpiBspListener *lis3dshBspListener;
};

#endif // LIS3DSHSPIBSP_H
