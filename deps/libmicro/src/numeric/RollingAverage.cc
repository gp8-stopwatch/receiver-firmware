/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "RollingAverage.h"

void RollingAverage::run (float in)
{
        queue.push_back_roll ();
        queue.back () = in;
}

/*****************************************************************************/

float RollingAverage::getResult () const
{
        float s = 0;
        size_t qSize = queue.size ();

        if (!qSize) {
                return 0.0f;
        }

        for (size_t i = 0; i < qSize; ++i) {
                s += queue.front (i);
        }

        return s / float(qSize);
}
