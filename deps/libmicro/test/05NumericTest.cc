/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "numeric/IncrementalAverage.h"
#include "numeric/RollingAverage.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>
#include <unistd.h>

TEST_CASE ("IncrementalAverage", "[numeric]")
{
        IncrementalAverage avg;
        REQUIRE (avg.getResult () == Approx (0.0f));

        avg.run (4);
        REQUIRE (avg.getResult () == 4.0f);

        avg.run (6);
        avg.run (3);
        avg.run (9);
        avg.run (4);
        avg.run (12);
        avg.run (4);
        avg.run (18);
        REQUIRE (avg.getResult () == Approx (7.5f));

        avg.run (7.5);
        REQUIRE (avg.getResult () == Approx (7.5f));

        avg.reset ();
}

TEST_CASE ("RollingAverage", "[numeric]")
{
        RollingAverage avg (8);
        REQUIRE (avg.getResult () == Approx (0.0f));

        avg.run (4);
        REQUIRE (avg.getResult () == 4.0f);

        avg.run (6);
        avg.run (3);
        avg.run (9);
        avg.run (4);
        avg.run (12);
        avg.run (4);
        avg.run (18);
        REQUIRE (avg.getResult () == Approx (7.5f));

        avg.run (7.5);
        REQUIRE (avg.getResult () == Approx (7.9375f));
}

