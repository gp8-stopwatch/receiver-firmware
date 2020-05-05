/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef INCREMENTALAVERAGE_H
#define INCREMENTALAVERAGE_H

#include <cstdlib>

class IncrementalAverage {
public:
        IncrementalAverage () : cnt (0), result (0) {}
        void run (float in) { result = result + (in - result) / ++cnt; }
        float getResult () const { return result; }
        void reset ();

private:
        size_t cnt;
        float result;
};

#endif // INCREMENTALAVERAGE_H
