/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_PIDCONTROLLER_H
#define LIB_MICRO_PIDCONTROLLER_H

class PidController {
public:
        PidController (float integralMax = 25.0f, float constOffset = 0.0f, float iScale = 1.0f, float dScale = 1.0f);
        float run (float in, float setPoint);

        float getKp () const { return kp; }
        void setKp (float value) { kp = value; }

        float getKi () const { return ki; }
        void setKi (float value) { ki = value; }

        float getKd () const { return kd; }
        void setKd (float value) { kd = value; }

        float getIntegralMax () const { return integralMax; }
        void setIntegralMax (float value) { integralMax = value; }

        float getConstOffset () const { return constOffset; }
        void setConstOffset (float value) { constOffset = value; }

        float getIScale () const { return iScale; }
        void setIScale (float value) { iScale = value; }

        float getDScale () const { return dScale; }
        void setDScale (float value) { dScale = value; }

        float getError () const { return error; }
        void setError (float value) { error = value; }

        float getIntegral () const { return integral; }
        void setIntegral (float value) { integral = value; }

        float getDerivative () const { return derivative; }
        void setDerivative (float value) { derivative = value; }

private:
        float kp, ki, kd;
        float error, integral, derivative;
        float integralMax, constOffset;
        float prevError;
        float iScale, dScale;
};

#endif // PIDCONTROLLER_H
