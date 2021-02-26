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

std::vector<DetectorEvent> events; // NOLINT

struct TestDetectorCallback : public IEdgeDetectorCallback {
        void report (DetectorEventType type, Result1us timePoint) override { events.push_back ({type, timePoint}); }
};

class SignalSimulator {
public:
        SignalSimulator (EdgeFilter &e) : edgeFilter{e} {}

        void signal (std::vector<uint32_t> const &edges, uint32_t runUntilMs = 0, std::optional<EdgePolarity> startPolarity = {});

        static constexpr uint32_t RUN_PERIOD_US = 5000;

private:
        EdgeFilter &edgeFilter;
        uint32_t currentTimeUs{};

        // Simulated edge of the signal. Flips from rising to falling indef.
        EdgePolarity currentPolarity{EdgePolarity::rising};
};

/****************************************************************************/

void SignalSimulator::signal (std::vector<uint32_t> const &edges, uint32_t runUntilMs, std::optional<EdgePolarity> startPolarity)
{
        if (startPolarity) {
                currentPolarity = *startPolarity;
        }

        for (auto t : edges) {
                // Call run as if it was running in the real main loop on the device

                // ms from now to the current edge we want to "input"
                auto msBeforeEdge = t - currentTimeUs;

                for (int i = 0; i < msBeforeEdge / RUN_PERIOD_US; ++i) {
                        edgeFilter.run (/* msToResult1us */ (currentTimeUs));
                        currentTimeUs += RUN_PERIOD_US;
                }

                // Adjust the time so it is exactly as stated in the function argument
                currentTimeUs = t;

                // Now the edge itself
                edgeFilter.onEdge ({/* msToResult1us */ (currentTimeUs), currentPolarity});

                // Flip the state.
                currentPolarity = (currentPolarity == EdgePolarity::rising) ? (EdgePolarity::falling) : (EdgePolarity::rising);
        }

        while (currentTimeUs < runUntilMs) {

                if (runUntilMs - currentTimeUs < RUN_PERIOD_US) {
                        currentTimeUs = runUntilMs;
                }
                else {
                        currentTimeUs += RUN_PERIOD_US;
                }

                edgeFilter.run (/* msToResult1us */ (currentTimeUs));
        }
}

/**
 * Events
 */
TEST_CASE ("Edge cases", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Simplest case 2 edges")
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
                sim.signal ({10000, 20000}, 30000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("Simplest case 3 edges")
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
                sim.signal ({10000, 20000, 30000}); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("Steady end, 1 noise")
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
                sim.signal ({10000, 15000, 15100, 20000}, 30000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10000);
        }

        SECTION ("Rising at the end, 1 noise")
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
                sim.signal ({10000, 15000, 15100, 20000, 30000}); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("1 negative, 1 positive spike")
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
                sim.signal ({10000, 15000, 15100, 20000, 25000, 25100}, 50000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("more spikes at the end")
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
                sim.signal ({10000, 15000, 15100, 20000, 25000, 25100, 30000, 30001, 50000, 50001}); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }
}

/**
 * Checks if after reporting a trigger event it does not repeat itself
 * for some reason. This was observed on the target.
 */
TEST_CASE ("Only one report", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("The only section")
        {
                /*
                 *        +-----+                  +-----+
                 *        |     |                  |     |
                 *        |     |                  |     |
                 *        |     |                  |     |
                 *        |     |                  |     |
                 * -------+     +-------+----------+     +------
                 * 0    10ms   22ms     34ms     6s+ 12ms
                 */
                sim.signal ({10000, 22000}, 34000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
                REQUIRE (events.back ().timePoint == 10000);

                sim.signal ({6040000, 6040000 + 12000}, 6040000 + 24000); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
                REQUIRE (events.back ().timePoint == 6040000);
        }
}

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
        SignalSimulator sim{edgeFilter};
        events.clear ();

        sim.signal ({10000, 15000, 15200, 15300, 15400, 25000, 35000}); // NOLINT

        // Current implementation cannot handle this scenario, and implementiung it would be hard. Current one should be tested for now.
        // REQUIRE (events.size () == 1);
        // REQUIRE (events.front ().type == DetectorEventType::trigger);
        // REQUIRE (events.front ().timePoint == 10 * 1000);
}

TEST_CASE ("Slightly more", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("First section")
        {
                /*
                 *        +-----+
                 *        |     |
                 *        |     |
                 *        |     |
                 *        |     |
                 * -------+     +-------+-----
                 * 0    10000  20001 30ms
                 */
                sim.signal ({10000, 20001}, 30002); // NOLINT
                REQUIRE (events.size () == 1);

                sim.signal ({}, 300010); // NOLINT
                REQUIRE (events.size () == 1);
        }
}

/**
 * Negative cases, where events are slightly (usually 1µs) shorter than
 * what is required to qualify as a valid event.
 */
TEST_CASE ("Slightly less", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Shorter high level")
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
                sim.signal ({10000, 19999}, 30000); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter low level")
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
                sim.signal ({10000, 20000}, 29999); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter high level, rising at the end")
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
                sim.signal ({10000, 19999, 30000}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter low level, rising at the end")
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
                sim.signal ({10000, 20000, 29999}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter high level, neg. spike")
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
                sim.signal ({10000, 15000, 15100, 19999}, 30000); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter low level, neg. spike")
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
                sim.signal ({10000, 15000, 15100, 20000}, 29999); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter high level, neg. spike, rising at the end")
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
                sim.signal ({10000, 15000, 15100, 19999, 30000}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Shorter low level, neg. spike, rising at the end")
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
                sim.signal ({10000, 15000, 15100, 20000, 29999}); // NOLINT
                REQUIRE (events.empty ());
        }
}

/**
 * As above, but more complicated signal plots.
 */
TEST_CASE ("Advanced / slightly less", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Negative and positive noise")
        {

                /*
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+--+
                 * 0    10ms   19999µ   35ms
                 */
                sim.signal ({10000, 15000, 15100, 19999, 25000, 25100}, 35100); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Negative and positive noise 2")
        {

                /*
                 *        +--+---+    +
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 *        |  |   |    |
                 * -------+  +   +----+--+
                 * 0    10ms   20ms   25,25+100   34999 + 100
                 *
                 * This basically is the same as the previous one, but before the
                 * unit test refactoiring it somehow made sense.
                 */
                sim.signal ({10000, 15000, 15100, 19999, 25000, 25100}, 34999 + 100); // NOLINT
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("Noise detection", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Negative burst")
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
                sim.signal ({10000}); // NOLINT
                REQUIRE (events.empty ());

                for (uint32_t i = 0; i < 750; ++i) {                     // NOLINT
                        sim.signal ({15000 + i * 2, 15000 + i * 2 + 1}); // NOLINT
                }

                sim.signal ({20000}); // NOLINT
                REQUIRE (events.empty ());

                sim.signal ({}, 1000000); // NOLINT
                REQUIRE (!events.empty ());
                REQUIRE (events.back ().type == DetectorEventType::noise);

                sim.signal ({}, 2000000); // NOLINT
                REQUIRE (events.back ().type == DetectorEventType::noNoise);
        }

        SECTION ("BURST, and then 1 positive")
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
                sim.signal ({10000}); // NOLINT
                REQUIRE (events.empty ());

                for (uint32_t i = 0; i < 750; ++i) {                     // NOLINT
                        sim.signal ({15000 + i * 2, 15000 + i * 2 + 1}); // NOLINT
                }

                sim.signal ({20000}); // NOLINT
                REQUIRE (events.empty ());

                sim.signal ({}, 1000000); // NOLINT
                REQUIRE (!events.empty ());
                REQUIRE (events.back ().type == DetectorEventType::noise);

                sim.signal ({1010000 + 1, 1010000 + 2}, 2000000); // NOLINT
                REQUIRE (events.back ().type == DetectorEventType::noNoise);
        }
}

/**
 * To verify a test conducted on the oscilloscope.
 */
TEST_CASE ("Spikes", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        /*
         *   + + + + + + +
         *   | | | | | | |
         *   | | | | | | |
         *   | | | | | | |
         *   | | | | | | |
         * --+-+-+-+-+-+-+-------------+
         * 0
         */
        for (uint32_t i = 0; i < 20; ++i) {                 // NOLINT
                uint32_t timePoint = i * 20000;             // Every 20ms NOLINT
                sim.signal ({timePoint, timePoint + 1000}); // NOLINT

                // edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                // edgeFilter.onEdge ({timePoint + 1000, EdgePolarity::falling});
                // edgeFilter.run (timePoint + 2000);
                // edgeFilter.run (timePoint + 5000);
                // edgeFilter.run (timePoint + 19000);
        }

        sim.signal ({}, 21 * 20000); // NOLINT
        REQUIRE (events.empty ());
}

TEST_CASE ("Noise level", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Basic test")
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
                sim.signal ({10000, 20000}, 30000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (edgeFilter.getNoiseLevel () == 0);
        }

        SECTION ("1 spike")
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
                sim.signal ({10000, 10100}, 1000'000); // NOLINT
                REQUIRE (edgeFilter.getNoiseLevel () == 1);
        }

        SECTION ("Many spikes of 1ms")
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
                for (uint32_t i = 0; i < 20; ++i) {                 // NOLINT
                        uint32_t timePoint = i * 20000;             // Every 20ms NOLINT
                        sim.signal ({timePoint, timePoint + 1000}); // NOLINT

                        // edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        // edgeFilter.onEdge ({timePoint + 1000, EdgePolarity::falling});
                }

                sim.signal ({}, 1000'000); // NOLINT
                REQUIRE (edgeFilter.getNoiseLevel () == 1);
        }

        SECTION ("Maximum noise")
        {
                /*
                 * Maximum noise - edge case - noise level not exceeding maximum
                 */
                uint32_t timePoint{};

                // NOLINTNEXTLINE 2 This is actually 2 seconds worth of signal
                for (uint32_t i = 0; i < msToResult1us (EdgeFilter::NOISE_CALCULATION_PERIOD_MS) / EdgeFilter::MIN_NOISE_SPIKE_1US + 10; ++i) {
                        timePoint = i * EdgeFilter::MIN_NOISE_SPIKE_1US * 2;                   // Every 20ms
                        sim.signal ({timePoint, timePoint + EdgeFilter::MIN_NOISE_SPIKE_1US}); // NOLINT
                }

                // This is only to recalulate the noise
                sim.signal ({}, timePoint + EdgeFilter::MIN_NOISE_SPIKE_1US + 1); // NOLINT
                REQUIRE (edgeFilter.getNoiseLevel () == 15);
        }

        SECTION ("Even more noise")
        {
                /*
                 * Maximum noise - noise 2 times more intense than max measurable (shorter spikes
                 * and more of them).
                 */
                auto noiseSpikeLen = EdgeFilter::MIN_NOISE_SPIKE_1US / 2;
                uint32_t timePoint{};

                for (uint32_t i = 0; i < msToResult1us (EdgeFilter::NOISE_CALCULATION_PERIOD_MS) / noiseSpikeLen + 10; ++i) { // NOLINT
                        uint32_t timePoint = i * noiseSpikeLen * 2;                                                           // Every 20ms
                        // sim.signal ({timePoint, timePoint + noiseSpikeLen});                                                  // NOLINT

                        edgeFilter.onEdge ({timePoint, EdgePolarity::rising});
                        edgeFilter.onEdge ({timePoint + noiseSpikeLen, EdgePolarity::falling});
                        edgeFilter.run (timePoint + noiseSpikeLen);
                }

                // This is only to recalulate the noise
                // sim.signal ({}, timePoint + noiseSpikeLen + 1); // NOLINT
                REQUIRE (edgeFilter.getNoiseLevel () == 15);
        }
}

TEST_CASE ("Duty cycle", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Duty cycle 100")
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
                getConfig ().setDutyTresholdPercent (100); // NOLINT
                sim.signal ({10000, 20000}, 30000);        // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("Duty cycle 100 - 2")
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
                getConfig ().setDutyTresholdPercent (100);        // NOLINT
                sim.signal ({10000, 15000, 15100, 20000}, 30000); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Duty cycle 100 - 3")
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
                getConfig ().setDutyTresholdPercent (100); // NOLINT
                sim.signal ({10000, 20000, 30000});        // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);
        }

        SECTION ("Duty cycle 100 - spike")
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
                getConfig ().setDutyTresholdPercent (100);        // NOLINT
                sim.signal ({10000, 15000, 15100, 20000, 30000}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Duty cycle 100 - pos. spike")
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
                getConfig ().setDutyTresholdPercent (100);        // NOLINT
                sim.signal ({10000, 20000, 25000, 25100, 30000}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Duty cycle 100 - 2 pos. spikes")
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
                getConfig ().setDutyTresholdPercent (100);                      // NOLINT
                sim.signal ({10000, 20000, 25000, 25100, 31000, 31050, 50000}); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Duty cycle 100 - 2,2 pos. spike")
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
                getConfig ().setDutyTresholdPercent (100);                      // NOLINT
                sim.signal ({10000, 20000, 25000, 25100, 35100, 35200}, 50000); // NOLINT
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("Duty cycle less", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Duty cycle 95")
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
                getConfig ().setDutyTresholdPercent (95);                                     // NOLINT
                sim.signal ({10000, 20000, 25000, 25100, 25200, 25300, 36000, 36000}, 50000); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Duty cycle 95, rising end")
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
                getConfig ().setDutyTresholdPercent (95);                                     // NOLINT
                sim.signal ({10000, 20000, 25000, 25100, 25200, 25300, 36000, 36000, 50000}); // NOLINT
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("No beam", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();
        getConfig ().setDutyTresholdPercent (50); // NOLINT

        SECTION ("First")
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
                sim.signal ({10000}, 1000'000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                sim.signal ({1500000}, 3000000); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                edgeFilter.run (3000001); // NOLINT
                REQUIRE (events.size () == 2);

                // Now test if when beam os restored, everything works as it shoud
                sim.signal ({3000000 + 100, 3000000 + 100 + 10000}, 3000000 + 100 + 20000); // NOLINT
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
        }

        SECTION ("No beam, noise")
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
                sim.signal ({10000, 11000, 11100, 12000, 12100, 13000, 13100}, 1010000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                sim.signal ({1500000, 1500000 + 11000, 1500000 + 11100, 1500000 + 12000, 1500000 + 12100, 1500000 + 13000, // NOLINT
                             1500000 + 13100});                                                                            // NOLINT
                REQUIRE (events.size () == 1);

                edgeFilter.run (3000000); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);
        }

        SECTION ("High level from the start")
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
                SignalSimulator sim{edgeFilter};
                events.clear ();

                sim.signal ({}, 60000); // NOLINT
                REQUIRE (events.empty ());

                sim.signal ({}, 1000'000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                sim.signal ({1500000}, 3000'000, EdgePolarity::falling); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                edgeFilter.run (3000001); // NOLINT
                REQUIRE (events.size () == 2);

                // Now test if when beam os restored, everything works as it shoud
                sim.signal ({3000000 + 100, 3000000 + 100 + 10000}, 3000000 + 100 + 20000); // NOLINT
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::trigger);
        }

        SECTION ("High level from the start, noise")
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

        SECTION ("Signal restored 2 times")
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
                sim.signal ({10000}, 1010000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::noBeam);

                sim.signal ({1500000}, 3000000); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);

                sim.signal ({3100000}, 4200000); // NOLINT
                REQUIRE (events.size () == 3);
                REQUIRE (events.back ().type == DetectorEventType::noBeam);

                sim.signal ({4200001}, 5300000); // NOLINT

                // edgeFilter.onEdge ({4200001, EdgePolarity::falling});
                // edgeFilter.run (5300000);
                REQUIRE (events.size () == 4);
                REQUIRE (events.back ().type == DetectorEventType::beamRestored);
        }
}

/*
 * Wrong behaviour of no beam tetector.
 */
TEST_CASE ("No beam not", "[detector]")
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
        SignalSimulator sim{edgeFilter};

        sim.signal ({9000, 9500, 10000, 15000}, 1500000); // NOLINT
        REQUIRE (events.empty ());
}

TEST_CASE ("No beam at the start", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::high};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();
        getConfig ().setDutyTresholdPercent (50); // NOLINT

        SECTION ("Simple")
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
                sim.signal ({20000}, 30000, EdgePolarity::falling); // NOLINT
                REQUIRE (events.empty ());
        }

        SECTION ("Noise")
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
                sim.signal ({1000, 1100, 2000, 2100, 3000, 3100}, 10000, EdgePolarity::falling); // NOLINT
                REQUIRE (events.empty ());

                sim.signal ({20000}, 30000); // NOLINT
                REQUIRE (events.empty ());
        }
}

TEST_CASE ("Blind period", "[detector]")
{
        TestDetectorCallback tc;
        EdgeFilter edgeFilter{EdgeFilter::PwmState::low};
        edgeFilter.setCallback (&tc);
        SignalSimulator sim{edgeFilter};
        events.clear ();

        SECTION ("Simplest blind")
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
                getConfig ().setBlindTime (1000); // NOLINT

                sim.signal ({10000, 20000}, 30000); // NOLINT
                REQUIRE (events.size () == 1);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10 * 1000);

                // Another valid trigger event, but occuring in the blind period.
                sim.signal ({40000, 50000}, 60000); // NOLINT
                REQUIRE (events.size () == 1);
        }

        SECTION ("No blind period, fastest 2 triggers")
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
                getConfig ().setBlindTime (0); // NOLINT

                sim.signal ({10000, 20000, 30000, 40000}, 50000); // NOLINT
                REQUIRE (events.size () == 2);
                REQUIRE (events.front ().type == DetectorEventType::trigger);
                REQUIRE (events.front ().timePoint == 10000);

                REQUIRE (events.back ().type == DetectorEventType::trigger);
                REQUIRE (events.back ().timePoint == 30000);
        }
}