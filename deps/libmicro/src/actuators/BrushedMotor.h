/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_BRUSHEDMOTOR_H
#define LIB_MICRO_BRUSHEDMOTOR_H

#include "Pwm.h"
#include <cstdint>

class Gpio;

/**
 * @brief The BrushedMotor class
 */
class BrushedMotor {
public:
        BrushedMotor (Gpio *dir, Pwm *p, Pwm::Channel channel, uint32_t fullScale)
            : direction (dir), pwm (p), channel (channel), factor (fullScale / 100), fullScale (fullScale), pwmInvert (false), directionInvert (false)
        {
        }

        /**
         * @brief setSpeed
         * @param speed from -100 to 100
         */
        void setSpeed (int32_t speed);
        void setPwmInvert (bool b) { pwmInvert = b; }
        void setDirectionInvert (bool b) { directionInvert = b; }

private:
        Gpio *direction;
        Pwm *pwm;
        Pwm::Channel channel;
        uint32_t factor;
        uint32_t fullScale;
        bool pwmInvert;
        bool directionInvert;
};

#endif // BRUSHEDMOTOR_H
