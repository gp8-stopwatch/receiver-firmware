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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};

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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                 *           15ms (100µs)
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+------+------+
                 * 0    10ms   20ms  25(100) 30ms    50ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                edgeFilter.run (30 * 1000 + 100);

                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);

                edgeFilter.run (50 * 1000);
                REQUIRE (events.size () == 1);
        }

        {

                /*
                 *           15ms (100µs)
                 *        +--+---+    +      +      +
                 *        |  |   |    |      |      |
                 *        |  |   |    |      |      |
                 *        |  |   |    |      |      |
                 *        |  |   |    |      |      |
                 * -------+  +   +----+------+------+
                 * 0    10ms   20ms  25(100) 30ms    50ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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

                edgeFilter.onEdge ({30000, EdgePolarity::rising});
                edgeFilter.onEdge ({30001, EdgePolarity::falling});

                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);

                edgeFilter.onEdge ({50000, EdgePolarity::rising});
                REQUIRE (events.size () == 1);
                edgeFilter.onEdge ({50001, EdgePolarity::falling});
                REQUIRE (events.size () == 1);
        }
}

/**
 * Checks if after reporting a trigger event it does not repeat itself
 * for some reason. This was observed on the target.
 */
TEST_CASE ("Only one report", "[detector]")
{
        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    10ms   22ms     34ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                events.clear ();
                edgeFilter.setCallback (&tc);

                Result1us time = 0;

                for (int i = 0; i < 10000 / 9; ++i) {
                        time += 9;
                        edgeFilter.run (time);
                }

                time = 10000;
                edgeFilter.onEdge ({time, EdgePolarity::rising});

                for (int i = 0; i < 12000 / 9; ++i) {
                        time += 9;
                        edgeFilter.run (time);
                }

                REQUIRE (events.empty ());

                time = 22000;
                edgeFilter.onEdge ({time, EdgePolarity::falling});
                REQUIRE (events.empty ());

                for (int i = 0; i < 12000 / 9; ++i) {
                        time += 9;
                        edgeFilter.run (time);
                }

                REQUIRE (events.size () == 1);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
                REQUIRE (events.back ().timePoint == 10000);

                for (int i = 0; i < 6 * 1000000 / 9; ++i) {
                        time += 9;
                        edgeFilter.run (time);
                }

                REQUIRE (events.size () == 1);

                // 6 seconds has elapsed, now I simulate the second trigger event

                time = 6040000;
                edgeFilter.onEdge ({time, EdgePolarity::rising});

                time += 12000;
                edgeFilter.onEdge ({time, EdgePolarity::falling});

                time += 12000;
                edgeFilter.run (time);

                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
                REQUIRE (events.back ().timePoint == 6040000);
        }
}

#if 0
TEST_CASE ("Low PWM in the middle", "[detector]")
{
        /*
         * This is a situation which shouldn't occur much if at all?
         * During long high period we have some disturbances. First comes
         * a long (200µs) negative spike, then 100µs positive, then another
         * short negative (100µs), and then the high state is restored.
         * The problem is, that in point "A" PWM of the slice (last 3 edges)
         * is LOW, and thus pwmState flips to low.
         *
         *           15ms
         *        +------+  +-+ +--------+         +
         *        |      |  | | |        |         |
         *        |      |  | | |        |         |
         *        |      |  | | |        |         |
         *        |      |  | | |        |         |
         * -------+      +--+ +-+        +---------+
         * 0    10ms          A         25ms     35ms
         */
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        events.clear ();

        // First rising @ 10ms
        edgeFilter.onEdge ({10000, EdgePolarity::rising});

        // Noise spike
        edgeFilter.onEdge ({15000, EdgePolarity::falling});
        edgeFilter.onEdge ({15000 + 200, EdgePolarity::rising});
        // Noise spike
        edgeFilter.onEdge ({15000 + 300, EdgePolarity::falling});
        edgeFilter.onEdge ({15000 + 400, EdgePolarity::rising});

        edgeFilter.onEdge ({25000, EdgePolarity::falling});

        edgeFilter.onEdge ({35000, EdgePolarity::rising});

        // Current implementation cannot handle this scenario, and implementiung it would be hard. Current one should be tested for now.
        // REQUIRE (events.size () == 1);
        // REQUIRE (events.front ().type == DetectorEventType::trigger);
        // REQUIRE (events.front ().timePoint == 10 * 1000);
}
#endif

TEST_CASE ("Slightly more", "[detector]")
{

        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    10000  20001 30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10000, EdgePolarity::rising});
                edgeFilter.run (10001);
                REQUIRE (events.empty ());

                // This high level is 1µs too short.
                edgeFilter.onEdge ({20001, EdgePolarity::falling});
                edgeFilter.run (20002);
                REQUIRE (events.empty ());

                // edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});
                edgeFilter.run (30002);
                REQUIRE (events.size () == 1);

                edgeFilter.run (300010);
                REQUIRE (events.size () == 1);

                edgeFilter.run (40003);
                REQUIRE (events.size () == 1);
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
                 * 0    10000  19999µ 30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // This high level is 1µs too short.
                edgeFilter.onEdge ({19999, EdgePolarity::falling});
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                for (int i = 0; i < 750; ++i) {
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                for (int i = 0; i < 750; ++i) {
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
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

TEST_CASE ("Noise level", "[detector]")
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
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};

                edgeFilter.setCallback (&tc);

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.run (20 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (30 * 1000);

                REQUIRE (events.size () == 1);
                REQUIRE (edgeFilter.getNoiseLevel () == 0);
        }

        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+
                 * 0    10ms   10ms+100µs  1s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({10100, EdgePolarity::falling});
                edgeFilter.run (10100);
                REQUIRE (events.empty ());

                edgeFilter.run (1000'000);

                REQUIRE (edgeFilter.getNoiseLevel () == 1);
        }

        {
                /*
                 *   + + + + + + +
                 *   | | | | | | |
                 *   | | | | | | |
                 *   | | | | | | |
                 *   | | | | | | |
                 * --+-+-+-+-+-+-+-------------+
                 * 0     every 20ms pulse 1ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                for (int i = 0; i < 20; ++i) {
                        Result1us timePoint = i * 20000; // Every 20ms
                        edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        edgeFilter.onEdge ({timePoint + 1000, EdgePolarity::falling});
                }

                edgeFilter.run (1000'000);
                REQUIRE (edgeFilter.getNoiseLevel () == 1);
        }

        {
                /*
                 * Maximum noise - edge case - noise level not exceeding maximum
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                for (int i = 0; i < msToResult1us (EdgeFilter::NOISE_CALCULATION_PERIOD_MS) / EdgeFilter::MIN_NOISE_SPIKE_1US + 10; ++i) {
                        Result1us timePoint = i * EdgeFilter::MIN_NOISE_SPIKE_1US * 2; // Every 20ms
                        edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        edgeFilter.onEdge ({timePoint + EdgeFilter::MIN_NOISE_SPIKE_1US, EdgePolarity::falling});
                        edgeFilter.run (timePoint + EdgeFilter::MIN_NOISE_SPIKE_1US);
                }

                REQUIRE (edgeFilter.getNoiseLevel () == 15);
        }

        {
                /*
                 * Maximum noise - noise 2 times more intense than max measurable (shorter spikes
                 * and more of them).
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                auto noiseSpikeLen = EdgeFilter::MIN_NOISE_SPIKE_1US / 2;

                for (int i = 0; i < msToResult1us (EdgeFilter::NOISE_CALCULATION_PERIOD_MS) / noiseSpikeLen + 10; ++i) {
                        Result1us timePoint = i * noiseSpikeLen * 2; // Every 20ms
                        edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        edgeFilter.onEdge ({timePoint + noiseSpikeLen, EdgePolarity::falling});
                        edgeFilter.run (timePoint + noiseSpikeLen);
                }

                REQUIRE (edgeFilter.getNoiseLevel () == 15);
        }
}

TEST_CASE ("Duty cycle", "[detector]")
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
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

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
                 *        +--+---+
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 *        |  |   |
                 * -------+  +   +-------+
                 * 0    10ms   20ms     30ms
                 */
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                // Noise spike (negative)
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({15 * 1000 + 1, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.run (30 * 1000);

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +------+       +
                 *        |      |       |
                 *        |      |       |
                 *        |      |       |
                 *        |      |       |
                 * -------+      +-------+
                 * 0    10ms   20ms     30ms
                 */
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                // // Noise spike (negative)
                // edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                // edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});

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
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                // Noise spike (negative)
                edgeFilter.onEdge ({15 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({15 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +------+   +   +
                 *        |      |   |   |
                 *        |      |   |   |
                 *        |      |   |   |
                 *        |      |   |   |
                 * -------+      +---+---+
                 * 0    10ms   20ms     30ms
                 */
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});

                // Noise spike (positive)
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({30 * 1000, EdgePolarity::rising});

                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +------+   +     +       +
                 *        |      |   |     |       |
                 *        |      |   |     |       |
                 *        |      |   |     |       |
                 *        |      |   |     |       |
                 * -------+      +---+-----+-------+
                 * 0    10ms   20ms        31ms    50ms
                 */
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});

                // Noise spike (positive)
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});

                edgeFilter.onEdge ({31 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({31 * 1000 + 50, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({50 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +------+   +         +
                 *        |      |   |         |
                 *        |      |   |         |
                 *        |      |   |         |
                 *        |      |   |         |
                 * -------+      +---+----++---+----+
                 * 0    10ms   20ms            35ms
                 *                      30,31ms     50ms
                 */
                getConfig ().setDutyTresholdPercent (100);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});

                // Noise spike (positive)
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});

                edgeFilter.run (30 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (31 * 1000);
                REQUIRE (events.empty ());

                // Noise spike (positive)
                edgeFilter.onEdge ({35 * 1000 + 100, EdgePolarity::rising});
                edgeFilter.onEdge ({35 * 1000 + 200, EdgePolarity::falling});

                edgeFilter.run (50 * 1000);
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("Duty cycle less", "[detector]")
{
        {

                /*
                 *        +------+   ++         +
                 *        |      |   ||         |
                 *        |      |   ||         |
                 *        |      |   ||         |
                 *        |      |   ||         |
                 * -------+      +---++----++---+----+
                 * 0    10ms   20ms            35ms
                 *                      30,31ms     50ms
                 */
                getConfig ().setDutyTresholdPercent (95);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});

                // Noise spike (positive)
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});
                edgeFilter.onEdge ({25 * 1000 + 200, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 300, EdgePolarity::falling});

                edgeFilter.run (30 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (31 * 1000);
                REQUIRE (events.empty ());

                // Noise spike (positive)
                edgeFilter.onEdge ({36 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({36 * 1000, EdgePolarity::falling});

                edgeFilter.run (50 * 1000);
                REQUIRE (events.empty ());
        }

        {

                /*
                 *        +------+   ++         +    +
                 *        |      |   ||         |    |
                 *        |      |   ||         |    |
                 *        |      |   ||         |    |
                 *        |      |   ||         |    |
                 * -------+      +---++----++---+----+
                 * 0    10ms   20ms            35ms
                 *                      30,31ms     50ms
                 */
                getConfig ().setDutyTresholdPercent (95);

                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});

                // Noise spike (positive)
                edgeFilter.onEdge ({25 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 100, EdgePolarity::falling});
                edgeFilter.onEdge ({25 * 1000 + 200, EdgePolarity::rising});
                edgeFilter.onEdge ({25 * 1000 + 300, EdgePolarity::falling});

                edgeFilter.run (30 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (31 * 1000);
                REQUIRE (events.empty ());

                // Noise spike (positive)
                edgeFilter.onEdge ({36 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({36 * 1000, EdgePolarity::falling});

                edgeFilter.onEdge ({50 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("No beam", "[detector]")
{
        {
                /*
                 *        +-----+         +--+
                 *        |     |         |  |
                 *        |     |         |  |
                 *        |     |         |  |
                 *        |     |         |  |
                 * -------+   + +-------+-+  +-----
                 * 0    10ms  1s10ms   3s  10ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.run (60000);

                edgeFilter.run (1010000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({1500000, EdgePolarity::falling});

                edgeFilter.run (3000000);
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                edgeFilter.run (3000001);
                REQUIRE (events.size () == 2);

                // Now test if when beam os restored, everything works as it shoud
                edgeFilter.onEdge ({3000000 + 100, EdgePolarity::rising});
                edgeFilter.onEdge ({3000000 + 100 + 10000, EdgePolarity::falling});
                edgeFilter.run (3000000 + 100 + 20000);
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
        }

        {
                /*
                 *        +--+---+-+-----+     +   + +
                 *        |  |   | |     |     |   | |
                 *        |  |   | |     |     |   | |
                 *        |  |   | |     |     |   | |
                 *        |  |   | |     |     |   | |
                 * -------+  +   + +  +  +-----+---+-+------+
                 * 0    10ms        1s10ms                  3s
                 *                       1,5s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();
                getConfig ().setDutyTresholdPercent (50);

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({11 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({11 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({12 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({12 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({13 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({13 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.run (1010000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({1500000, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 11 * 1000, EdgePolarity::rising});
                REQUIRE (events.size () == 1);
                edgeFilter.onEdge ({1500000 + 11 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 12 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({1500000 + 12 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 13 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({1500000 + 13 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.run (3000000);
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);
        }

        {
                /*
                 *--------------+         +--+
                 *              |         |  |
                 *              |         |  |
                 *              |         |  |
                 *              |         |  |
                 *            + +-------+-+  +-----
                 * 0           1s10ms   3s  10ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::high};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.run (10000);
                REQUIRE (events.empty ());

                edgeFilter.run (60000);
                REQUIRE (events.empty ());

                edgeFilter.run (1000000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({1500000, EdgePolarity::falling});

                edgeFilter.run (3000000);
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                edgeFilter.run (3000001);
                REQUIRE (events.size () == 2);

                // Now test if when beam os restored, everything works as it shoud
                edgeFilter.onEdge ({3000000 + 100, EdgePolarity::rising});
                edgeFilter.onEdge ({3000000 + 100 + 10000, EdgePolarity::falling});
                edgeFilter.run (3000000 + 100 + 20000);
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
        }

        {
                /*
                 *----------+---+-+-----+     +   + +
                 *          |   | |     |     |   | |
                 *          |   | |     |     |   | |
                 *          |   | |     |     |   | |
                 *          |   | |     |     |   | |
                 *          +   + +  +  +-----+---+-+------+
                 * 0    10ms        1s10ms                  3s
                 *                       1,5s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::high};
                edgeFilter.setCallback (&tc);
                events.clear ();
                getConfig ().setDutyTresholdPercent (50);

                edgeFilter.run (10000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({11 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({11 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({12 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({12 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.onEdge ({13 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({13 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.run (1010000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({1500000, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 11 * 1000, EdgePolarity::rising});
                REQUIRE (events.size () == 1);
                edgeFilter.onEdge ({1500000 + 11 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 12 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({1500000 + 12 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.onEdge ({1500000 + 13 * 1000, EdgePolarity::rising});
                edgeFilter.onEdge ({1500000 + 13 * 1000 + 100, EdgePolarity::falling});
                REQUIRE (events.size () == 1);

                edgeFilter.run (3000000);
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);
        }

        {
                /*
                 *        +-----+         +------+
                 *        |     |         |      |
                 *        |     |         |      |
                 *        |     |         |      |
                 *        |     |         |      |
                 * -------+   + +-------+-+      +-----
                 * 0    10ms  1s10ms   3s 4s     5s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({10 * 1000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.run (60000);

                edgeFilter.run (1010000);
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({1500000, EdgePolarity::falling});

                edgeFilter.run (3000000);
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                edgeFilter.run (3000001);
                REQUIRE (events.size () == 2);

                edgeFilter.onEdge ({3100000, EdgePolarity::rising});

                edgeFilter.run (4200000);
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::noBeam);

                edgeFilter.onEdge ({4200001, EdgePolarity::falling});
                edgeFilter.run (5300000);
                REQUIRE (events.size () == 4);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);
        }
}

/*
 * Wrong behaviour of no beam tetector.
 */
TEST_CASE ("No beam not", "[detector]")
{
        {
                /*
                 *     +-+  +-----+
                 *     | |  |     |
                 *     | |  |     |
                 *     | |  |     |
                 *     | |  |     |
                 * ----+ +--+     +-----------+
                 * 0       10ms  15ms          1.5s
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({9000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({9500, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({10000, EdgePolarity::rising});
                REQUIRE (events.empty ());

                edgeFilter.run (11000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({15000, EdgePolarity::falling});
                REQUIRE (events.empty ());

                edgeFilter.run (25000);
                edgeFilter.run (35000);

                edgeFilter.run (1500000);
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("No beam at the start", "[detector]")
{
        {
                /*
                 * ------------+
                 *             |
                 *             |
                 *             |
                 *             |
                 *       +     +-------+
                 * 0     10    20ms    30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::high};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.run (20 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (30 * 1000);
                REQUIRE (events.empty ()); // TODO I don't know how to overcome this yet. Generally speaking this is not an error.
        }

        {
                /*
                 * ---+-+--+---------+
                 *    | |  |         |
                 *    | |  |         |
                 *    | |  |         |
                 *    | |  |         |
                 *    + +  +   +     +-------+
                 * 0          10ms   20ms     30ms
                 */
                TestDetectorCallback tc;
                EdgeFilter edgeFilter{EdgeFilter::PwmState::high};
                edgeFilter.setCallback (&tc);
                events.clear ();

                edgeFilter.onEdge ({1 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({1 * 1000 + 100, EdgePolarity::rising});
                edgeFilter.onEdge ({2 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({2 * 1000 + 100, EdgePolarity::rising});
                edgeFilter.onEdge ({3 * 1000, EdgePolarity::falling});
                edgeFilter.onEdge ({3 * 1000 + 100, EdgePolarity::rising});

                edgeFilter.run (10 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.onEdge ({20 * 1000, EdgePolarity::falling});
                edgeFilter.run (20 * 1000);
                REQUIRE (events.empty ());

                edgeFilter.run (30 * 1000);
                REQUIRE (events.empty ());
        }
}