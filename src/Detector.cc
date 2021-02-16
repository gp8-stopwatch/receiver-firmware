/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Detector.h"

#ifndef UNIT_TEST
#include "Gpio.h"
Gpio senseOn{GPIOB, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL};
#else
#define __disable_irq(x) x
#define __enable_irq(x) x
#endif

/*****************************************************************************/

bool EdgeFilter::onEdge (Edge const &e)
{
        // Adds the event to the queue, checks for trigger event
        if (!EdgeDetector::onEdge (e)) {
                return false;
        }

        /*--------------------------------------------------------------------------*/

        // Calculate duty cycle of present slice of the signal
        Result1us cycleTreshold = (queue.back ().timePoint - queue.front ().timePoint)
                * dutyTresholdPercent;                                  // slice length times treshold. See equation in the docs.
        Result1us hiDuration = queue[1].timePoint - queue[0].timePoint; // We assume for now, that queue[0] is rising
        Result1us lowDuration = queue[2].timePoint - queue[1].timePoint;

        /*--------------------------------------------------------------------------*/

        // Find out which edge in the queue is rising, which falling.
        auto *firstRising = &queue[0]; // Pointers are smaller than Edge object
        auto *firstFalling = &queue[1];

        // 50% chance that above are correct. If our assumption wasn't right, we swap.
        if (firstRising->polarity != EdgePolarity::rising) {
                std::swap (firstRising, firstFalling);
                std::swap (hiDuration, lowDuration);
        }

        /*--------------------------------------------------------------------------*/

        if (hiDuration * 100
            >= cycleTreshold) { // Tu może nie złapać kiedy długo było low, a potem high przez 11ms. Duty będzie na low, a był event.
                if (state == State::high) {
                        return false;
                }

                state = State::high;
                highStateStart = firstRising->timePoint;
                // lastStateChange = firstRising->timePoint;
#ifndef UNIT_TEST
                senseOn.set (true);
#endif

                if (next != nullptr) {
                        // next->onEdge ({highStateStart, EdgePolarity::rising});
                        return next->onEdge (*firstRising);
                }
        }
        else if (lowDuration * 100 >= cycleTreshold) {
                if (state == State::low) {
                        return false;
                }

                state = State::low;
                lowStateStart = firstFalling->timePoint;
                // lastStateChange = firstFalling->timePoint;
#ifndef UNIT_TEST
                senseOn.set (false);
#endif

                if (next != nullptr) {
                        // next->onEdge ({lowStateStart, EdgePolarity::falling});
                        return next->onEdge (*firstFalling);
                }
        }

        return true;
}

/****************************************************************************/

void EdgeFilter::run (Result1us const &now)
{
        // TODO critical
        if (queue.empty ()) {
                return;
        }

        // TODO critical
        auto &back = queue.back ();

        // if (now - getLastStateChange () /* lastStateChange */ >= msToResult1 (minTreggerEventMs)) {
        // if (state == State::high && queue.back().polarity == EdgePolarity::falling &&  now - getLastStateChange () /* lastStateChange */ >=
        // msToResult1 (minTreggerEventMs)) {

        if (now - back.timePoint >= msToResult1 (minTreggerEventMs)) {
                __disable_irq ();

                if (back.polarity == EdgePolarity::falling) {
                        onEdge ({now, EdgePolarity::rising});
                        onEdge ({now, EdgePolarity::falling});
                }

                // if (state == State::high) {
                //         // if (back.polarity == EdgePolarity::rising) {
                //         onEdge ({now, EdgePolarity::falling});
                //         onEdge ({now, EdgePolarity::rising});
                // }
                // else {
                //         onEdge ({now, EdgePolarity::rising});
                //         onEdge ({now, EdgePolarity::falling});
                // }
                __enable_irq ();
        }

        // if (highStateStart < lowStateStart && lowStateStart - highStateStart >= msToResult1 (minTreggerEventMs)
        //     && now - lowStateStart >= msToResult1 (minTreggerEventMs)) {
        //         callback->report (DetectorEventType::trigger, queue[0].timePoint);
        // }

        if (next != nullptr) {
                next->run (now);
        }
}
