/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "PidController.h"

/*****************************************************************************/

PidController::PidController (float integralMax, float constOffset, float iScale, float dScale)
    : kp (0),
      ki (0),
      kd (0),
      error (0),
      integral (0),
      derivative (0),
      integralMax (integralMax),
      constOffset (constOffset),
      prevError (0),
      iScale (iScale),
      dScale (dScale)
{
}

/*****************************************************************************/

float PidController::run (float input, float setPoint)
{
        error = constOffset + setPoint - input;
        integral += error * iScale;

        // Simple anti integral windup.
        if (integral > integralMax) {
                integral = integralMax;
        }
        else if (integral < -integralMax) {
                integral = -integralMax;
        }

        derivative = (error - prevError) / dScale;
        float out = kp * error + ki * integral + kd * derivative;
        prevError = error;
        return out;
}
