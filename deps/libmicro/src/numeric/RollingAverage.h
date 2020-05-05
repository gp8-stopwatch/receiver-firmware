/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ROLLINGAVERAGE_H
#define ROLLINGAVERAGE_H

#include "collection/Queue.h"
#include <cstdlib>

class RollingAverage {
public:
        RollingAverage (size_t maxSiz) : queue (maxSiz) {}
        void run (float in);
        float getResult () const;
        void reset () { queue.clear (); }

private:
        Queue<float> queue;
};

#endif // ROLLINGAVERAGE_H
