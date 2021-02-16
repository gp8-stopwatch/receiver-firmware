/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "../Detector.h"
#include "catch.hpp"
#include <vector>

struct DetectorEvent {
        DetectorEventType type{};
        Result1us timePoint{};
};

std::vector<DetectorEvent> events;

struct TestDetectorCallback : public IEdgeDetectorCallback {
        void report (DetectorEventType type, Result1us timePoint) override { events.push_back ({type, timePoint}); }
};

/**
 *
 */
TEST_CASE ("First test", "[detector]")
{

        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeDetector triggerDetector{};
                EdgeFilter edgeFilter{&triggerDetector, EdgeFilter::State::low};

                edgeFilter.setCallback (&tc);

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.run (20 * 1000);
                REQUIRE (events.empty ());

                // edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                edgeFilter.run (30 * 1000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        {
                /*
                 *        +-----+       +
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 * -------+     +-------+
                 * 0    10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeDetector triggerDetector{};
                triggerDetector.setCallback (&tc);

                EdgeFilter edgeFilter{&triggerDetector, EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.run (20 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                // edgeFilter.run (30 * 1000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        {

                /*
                 *        +--+---+       +
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 * -------+  +   +-------+
                 * 0    10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeDetector triggerDetector{};
                triggerDetector.setCallback (&tc);

                EdgeFilter edgeFilter{&triggerDetector, EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative)
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Trzeba uwzględniać aktualną długość leveli zarówno w onEvent jaki w run. Jeżeli dłigość przekroczy min Event, to zmieniać
                // automatycznie stan i ustawiać highStateStart i lowStateStaty

                // edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                // edgeFilter.onEdge ({30 * 1000 + 100, EdgePolarity::falling});
                edgeFilter.run (30 * 1000);

                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }
}