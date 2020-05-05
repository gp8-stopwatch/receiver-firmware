/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <cstdint>
#include <stm32f0xx_hal.h>

extern "C" void TIM14_IRQHandler ();
class FastStateMachine;
struct IDisplay;

class StopWatch {
public:
        StopWatch ();
        virtual ~StopWatch () {}

        static StopWatch *singleton ()
        {
                static StopWatch st;
                return &st;
        }

        void init ();
        void reset () { time = 0; }
        void start () { running = true; }
        void stop () { running = false; }
        unsigned int getTime () const { return time; }

        void setDisplay (IDisplay *d) { this->display = d; }
        void setStateMachine (FastStateMachine *s) { this->stateMachine = s; }

        const unsigned int MAX_TIME = 100u * 60u * 100u;

private:
        friend void TIM14_IRQHandler ();
        void onInterrupt ();

private:
        IDisplay *display;
        TIM_HandleTypeDef stopWatchTimHandle;
        bool running;
        unsigned int time = 0;
        FastStateMachine *stateMachine;
};

#endif // STOPWATCH_H
