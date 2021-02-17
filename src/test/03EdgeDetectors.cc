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
 * Events
 */
TEST_CASE ("Edge cases", "[detector]")
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
                EdgeFilter edgeFilter{EdgeFilter::State::low};

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
                EdgeFilter edgeFilter{EdgeFilter::State::low};
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
                 *        +--+---+
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 * -------+  +   +-------+
                 * 0    10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
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

                edgeFilter.run (30 * 1000);

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
                EdgeFilter edgeFilter{EdgeFilter::State::low};
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

                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});

                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        {

                /*
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+--+
                 * 0    10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                // First rising @ 10ms
                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative) @ 15ms
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Falling @ 20ms
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Noise spike (positive) @ 25ms
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Run after minTreggerEventMs from last edge
                edgeFilter.run (35 * 1000 + 100);

                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }
}

/**
 * Negative cases, where events are slightly (usually 1µs) shorter than
 * what is required to qualify as a valid event.
 */
TEST_CASE ("Slightly less", "[detector]")
{

        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    9999µ  2*9999µ 30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({9999, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // This high level is 1µs too short.
                edgeFilter.onEdge ({9999 * 2, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                edgeFilter.run (30 * 1000);

                REQUIRE (events.empty ());
        }

        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    10ms   20ms     29999µ
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                edgeFilter.run (20 * 1000 + 9999);

                REQUIRE (events.size () == 0);
        }

        {
                /*
                 *        +-----+       +
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 * -------+     +-------+
                 * 0    10ms   19999µ  30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // 1µs too short
                edgeFilter.onEdge ({19999, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                REQUIRE (events.size () == 0);
        }

        {
                /*
                 *        +-----+       +
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 *        |     |       |
                 * -------+     +-------+
                 * 0    10ms   20ms  29999µ
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // 1µs too short
                edgeFilter.onEdge ({29999, EdgePolarity::rising});
                REQUIRE (events.size () == 0);
        }

        {

                /*
                 *        +--+---+
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 * -------+  +   +-------+
                 * 0    10ms   19999ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative)
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({19999, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.run (30 * 1000);

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +--+---+
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 * -------+  +   +-------+
                 * 0    10ms   20ms     29999µ
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
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

                edgeFilter.run (29999);

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +--+---+       +
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 * -------+  +   +-------+
                 * 0    10ms   19999µ   30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative)
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({19999, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +--+---+       +
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 *        |  |   |       |
                 * -------+  +   +-------+
                 * 0    10ms   20ms     29999µ
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
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

                edgeFilter.onEdge ({29999, EdgePolarity::rising});

                REQUIRE (events.empty ());
        }
}

/**
 * As above, but more complicated signal plots.
 */
TEST_CASE ("Advanced / slightly less", "[detector]")
{
        {

                /*
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+--+
                 * 0    10ms   19999µ   30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                // First rising @ 10ms
                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative) @ 15ms
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Falling @ 20ms
                edgeFilter.onEdge ({19999, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Noise spike (positive) @ 25ms
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Run after minTreggerEventMs from last edge
                edgeFilter.run (35 * 1000 + 100);
                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+--+
                 * 0    10ms   20ms   25,25+100   34999 + 100
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                // First rising @ 10ms
                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative) @ 15ms
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Falling @ 20ms
                edgeFilter.onEdge ({19999, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Noise spike (positive) @ 25ms
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.empty ());

                // Run after minTreggerEventMs from last edge
                edgeFilter.run (34999 + 100);
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("Noise detection", "[detector]")
{

        {
                /*
                 *        +--+++++---+
                 *        |  |||||   |
                 *        |  |||||   |
                 *        |  |||||   |
                 *        |  |||||   |
                 * -------+  +++++   +-------+------+
                 * 0    10ms        20ms     1s     2s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                for (int i = 0; i < 5; ++i) {
                        edgeFilter.onEdge ({static_cast<Result1us> (15 * 1000 + i * 2), EdgePolarity::falling});
                        edgeFilter.onEdge ({static_cast<Result1us> (15 * 1000 + i * 2 + 1), EdgePolarity::rising});
                }

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.run (1000 * 1000);

                REQUIRE (!events.empty ());
                REQUIRE (events.back ().type == DetectorEventType::noise);

                edgeFilter.run (2000 * 1000);
                REQUIRE (events.back ().type == DetectorEventType::noNoise);

                // REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        {
                /*
                 *        +--+++++---+           +
                 *        |  |||||   |           |
                 *        |  |||||   |           |
                 *        |  |||||   |           |
                 *        |  |||||   |           |
                 * -------+  +++++   +-------+---+--+
                 * 0    10ms        20ms     1s     2s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                for (int i = 0; i < 5; ++i) {
                        edgeFilter.onEdge ({static_cast<Result1us> (15 * 1000 + i * 2), EdgePolarity::falling});
                        edgeFilter.onEdge ({static_cast<Result1us> (15 * 1000 + i * 2 + 1), EdgePolarity::rising});
                }

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.run (1000 * 1000);

                REQUIRE (!events.empty ());
                REQUIRE (events.back ().type == DetectorEventType::noise);

                edgeFilter.onEdge ({1010000 + 1, EdgePolarity::rising});
                edgeFilter.onEdge ({1010000 + 2, EdgePolarity::falling});

                edgeFilter.run (2000 * 1000);
                REQUIRE (events.back ().type == DetectorEventType::noNoise);

                // REQUIRE (events.front ().timePoint == 10 * 1000);
        }
}

/**
 * To verify a test conducted on the oscilloscope.
 */
TEST_CASE ("Spikes", "[detector]")
{

        {
                /*
                 *   + + + + + + +
                 *   | | | | | | |
                 *   | | | | | | |
                 *   | | | | | | |
                 *   | | | | | | |
                 * --+-+-+-+-+-+-+-------------+
                 * 0
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::State::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                for (int i = 0; i < 20; ++i) {
                        Result1us timePoint = i * 20000; // Every 20ms
                        edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        edgeFilter.onEdge ({timePoint + 1000, EdgePolarity::falling});
                        edgeFilter.run (timePoint + 2000);
                        edgeFilter.run (timePoint + 5000);
                        edgeFilter.run (timePoint + 19000);
                }

                REQUIRE (events.empty ());
        }
}