/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_BIPOLARSTEPPER_H
#define LIB_MICRO_BIPOLARSTEPPER_H

#include "Pwm.h"
#include <cstdint>

class Gpio;

// TODO przeliczanie okresów timera na prędkość obrotową jakoś we zewnętrznej funkcji, albo nawet w klasie timera, czy PWM.
// TODO ramp profiles, na jakimjś prostym interfejsie jak tween albo coś.
// TODO one nie mogą angażować CPU! To musi działać niezaleznie od CPU!

struct IStepperMotor {

        virtual ~IStepperMotor () = default;

        enum MicroStepping : int { STEP_FULL, STEP_HALF, STEP_4, STEP_8, STEP_16, STEP_32, STEP_64, STEP_128, STEP_256 };
        virtual void setMicroStepping (MicroStepping ms) = 0;
        virtual MicroStepping getMicroStepping () const = 0;

        /**
         * @brief Spins the motor n steps. When n is negative motor spins in one direction, and
         * when positive in another direction.
         */
        virtual void step (int n) = 0;

        /**
         * @brief Sets speed of movement from 0 (minumum) to 100 (maximum).
         * @param speed
         */
        using Speed = uint8_t;
        virtual void setSpeed (Speed speed) = 0;
};

// struct AbstractStepperMotor : public IStepperMotor {

//        virtual ~AbstractStepperMotor () = default;

//        enum MicroStepping : int { STEP_FULL, STEP_HALF, STEP_4, STEP_8, STEP_16, STEP_32, STEP_64, STEP_128, STEP_256 };
//        void setMicroStepping (MicroStepping ms);
//        MicroStepping getMicroStepping () const;

//        // void step (int n);
// private:
//};

/**
 * Full step.
 * TODO
 * - ramp profile(s)
 * - n steps movement
 * - microstepping (someday)
 * - setSpeed to act lineary, from -100 to 100.
 *
 * Remrkas:
 * - Linear acceleration control is harder to achieve than I thought. Links that helped me:
 * AVR446 (http://www.microchip.com/wwwappnotes/appnotes.aspx?appnote=en591185)
 */
class Drv8835StepperMotor {
public:
        Drv8835StepperMotor (Gpio *ap, Gpio *ae, Gpio *bp, Gpio *be, uint16_t steps)
            : aPhasePin (ap), aEnablePin (ae), bPhasePin (bp), bEnablePin (be), stepsNo (steps), cycle (0), speed (0), steps (0), counter (0)
        {
        }

        void setSpeed (int32_t speed);

        /// Call from a timer or loop.
        void timeStep ();
        // private:

        void power (bool on);

        /// Move n steps (- or +).
        // void step (int n);

        void stepLeft ();
        void stepRight ();

private:
        Gpio *aPhasePin;
        Gpio *aEnablePin;
        Gpio *bPhasePin;
        Gpio *bEnablePin;
        uint16_t stepsNo;
        uint8_t cycle;
        int speed;
        int steps;
        uint32_t counter;
};

using BipolarStepper = Drv8835StepperMotor;

/**
 * @brief The Stspin820StepperMotor class
 */
class Stspin820StepperMotor : public IStepperMotor {
public:
        Stspin820StepperMotor (Pwm &p, Gpio &dir, Gpio *m0 = nullptr, Gpio *m1 = nullptr, Gpio *m2 = nullptr)
            : pwm (p), direction (dir), mode0 (m0), mode1 (m1), mode2 (m2)
        {
        }

        void setMicroStepping (MicroStepping ms) override final{}
        MicroStepping getMicroStepping () const override final{}

        void step (int n) override final;
        void setSpeed (Speed speed) override final;

private:
        Pwm &pwm;
        Gpio &direction;
        Gpio *mode0;
        Gpio *mode1;
        Gpio *mode2;
        Pwm::Channel channel = Pwm::CHANNEL2;
};

#endif // BIPOLARSTEPPER_H
