/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_HARDWARETIMER_H
#define LIB_MICRO_HARDWARETIMER_H

#include "Hal.h"
#include <functional>

extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler ();
extern "C" void TIM2_IRQHandler ();
extern "C" void TIM3_IRQHandler ();
extern "C" void TIM15_IRQHandler ();

class HardwareTimer;

class AbstractTimerChannel {
public:
        AbstractTimerChannel (HardwareTimer *timer, int channelNumber);
        virtual ~AbstractTimerChannel () = default;

        void setOnIrq (std::function<void(void)> const &onIrq) { this->onIrq = onIrq; }

        /**
         * Converts channel number 0, 1, 2 etc to HAL channel number like TIM_CHANNEL_1, TIM_CHANNEL_2 respectively
         */
        static int channelNumberToHal (int number);

protected:
        friend class HardwareTimer;
        std::function<void(void)> onIrq;
};

class OutputCompareChannel : public AbstractTimerChannel {
public:
        virtual ~OutputCompareChannel () = default;
};

class PwmChannel : public AbstractTimerChannel {
public:
        virtual ~PwmChannel () = default;
        virtual void onIrq () = 0;
};

/**
 * @brief The InputCaptureChannel class
 */
class InputCaptureChannel : public AbstractTimerChannel {
public:
        InputCaptureChannel (HardwareTimer *timer, int channelNumber, bool withIrq = false);
        virtual ~InputCaptureChannel () = default;
};

/**
 * @brief The HardwareTimer class
 */
class HardwareTimer {
public:
        HardwareTimer (TIM_TypeDef *instance, uint32_t prescaler, uint32_t period);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkEnable (TIM_HandleTypeDef *timer);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkDisable (TIM_HandleTypeDef *timer);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (&htim); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (&htim); }

        // TODO use 0, 1, 2, 3 raw numers instead of thi enum, to be consistent with channle classes
        enum Channel { CHANNEL1 = 0x01, CHANNEL2 = 0x02, CHANNEL3 = 0x04, CHANNEL4 = 0x08 };

        /**
         * Use bitmask Channel enum values to enable channels.
         * @param channels
         */
        void enableChannels (uint8_t channels);

        void setDuty (uint8_t channel, uint32_t duty);

        void setOnUpdate (std::function<void(void)> const &o) { onUpdate = o; }

private:
        void setChannel (size_t i, AbstractTimerChannel *ch) { channel[i] = ch; }

private:
        std::function<void(void)> onUpdate;
        TIM_HandleTypeDef htim;
        AbstractTimerChannel *channel[4];

        // TODO some ifdefs.
        static HardwareTimer *timer1;
        static HardwareTimer *timer2;
        static HardwareTimer *timer3;
        static HardwareTimer *timer15;
        static void serviceIrq (HardwareTimer *that);

        friend void TIM1_BRK_UP_TRG_COM_IRQHandler ();
        friend void TIM2_IRQHandler ();
        friend void TIM3_IRQHandler ();
        friend void TIM15_IRQHandler ();
        friend class AbstractTimerChannel;
        friend class OutputCompareChannel;
        friend class InputCaptureChannel;
};

#endif // HARDWARETIMER_H
