/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "../detectors/Detector.h"
#include <vector>

// struct DetectorEvent {
//         DetectorEventType type{};
//         Result1us timePoint{};
// };

// std::vector<DetectorEvent> events;

struct TestDetectorCallback : public IEdgeDetectorCallback {
        void report (DetectorEventType type, Result1us timePoint) override
        { /* events.push_back ({type, timePoint}); */
        }
};

int main ()
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);

        // Noise
        for (Result1us i = 0; i < 10000; ++i) {
                edgeFilter.onEdge ({i * 200, EdgePolarity::rising});
                edgeFilter.onEdge ({i * 200 + 100, EdgePolarity::falling});
                // edgeFilter.run (i * 200 + 100);
        }
}