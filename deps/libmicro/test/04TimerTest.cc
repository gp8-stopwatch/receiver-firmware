/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Timer.h"
#include "catch.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>
#include <unistd.h>

TEST_CASE ("Timer", "[timer]")
{
        Timer t;
        REQUIRE (t.isExpired ());

        t.start (50);
        REQUIRE (!t.isExpired ());

        std::this_thread::sleep_for (std::chrono::milliseconds (40));
        REQUIRE (!t.isExpired ());
        REQUIRE (t.elapsed () == Approx (40).epsilon (5));

        std::this_thread::sleep_for (std::chrono::milliseconds (15));
        REQUIRE (t.isExpired ());

        std::this_thread::sleep_for (std::chrono::milliseconds (100));
        REQUIRE (t.isExpired ());
}

TEST_CASE ("usleep", "[timer]")
{
        Timer t;
        REQUIRE (t.isExpired ());

        t.start (50);
        REQUIRE (!t.isExpired ());

        usleep (40 * 1000);
        REQUIRE (!t.isExpired ());
        REQUIRE (t.elapsed () == Approx (40).epsilon (5));

        usleep (15 * 1000);
        REQUIRE (t.isExpired ());
        REQUIRE (t.elapsed () == Approx (55).epsilon (5));

        usleep (100 * 1000);
        REQUIRE (t.isExpired ());
}

