/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "collection/CircularQueue.h"
#include <iostream>
#include <string>

/*****************************************************************************/

/**
 * @brief
 */
TEST_CASE ("push", "[ccq2]")
{
        CharacterCircularQueue<16> buf;
        buf.pushBack ("ala");
        REQUIRE (buf.front ().first == std::string ("ala"));

        buf.pushBack ("ma");
        REQUIRE (buf.front ().first == std::string ("ala"));
}

/**
 * @brief
 */
TEST_CASE ("push+pop", "[ccq2]")
{
        CharacterCircularQueue<16> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("ala"));

        /*  o         i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0| | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.front ().second == 3);
        REQUIRE (buf.pushBack ("ma"));

        /*  o                 i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0|3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.popFront ());

        /*            o       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.pushBack ("kota"));

        /*            o                   i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0|5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.popFront ());

        /*                    o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.pushBack ("oraz"));

        /*              i     o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | |5|k|o|t|a|0|E|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.popFront ());

        /*  o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (buf.pushBack ("psaA"));

        /*  o                       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0| | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("abc"));
        REQUIRE (buf.pushBack ("ab"));

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("ab"));
        REQUIRE (buf.popFront ());

        /*  i           o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | |5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("psaA"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push+pop2", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        REQUIRE (buf.isEmpty ());
        REQUIRE (buf.pushBack ("a"));
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.pushBack ("b"));
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.pushBack ("c"));
        REQUIRE (buf.getSize () == 3);
        REQUIRE (!buf.pushBack ("d"));

        REQUIRE (buf.front ().first == std::string ("a"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("b"));
        REQUIRE (buf.pushBack ("d"));
        REQUIRE (buf.getSize () == 3);

        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("c"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.front ().first == std::string ("d"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 0);
        REQUIRE (buf.isEmpty ());
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push to much", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("1234567"));
        REQUIRE (buf.popFront ());
        REQUIRE (!buf.pushBack ("12345678"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push and remove at once", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("12"));

        /*  o       i
         * +-+-+-+-+-+-+-+-+-+
         * |3|1|2|0| | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("12"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*         io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("34"));

        /*          o       i
         * +-+-+-+-+-+-+-+-+-+
         * | | | | |2|3|4|0| |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("34"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*                 io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("45"));
        REQUIRE (buf.front ().first == std::string ("45"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("67"));
        REQUIRE (buf.front ().first == std::string ("67"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());
}

TEST_CASE ("push and remove instantly", "[ccq2]")
{
        CharacterCircularQueue<256> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().first == std::string ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().second == 72);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().first == std::string ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().second == 74);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().first == std::string ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().second == 79);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().first == std::string ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().second == 80);
        REQUIRE (buf.popFront ());
}

/**
 * @brief
 */
TEST_CASE ("push byte", "[ccq2]")
{
        {
                CircularQueue<char, 16> buf;
                buf.pushBack ("ala");
                REQUIRE (buf.front ().first == std::string ("ala"));

                buf.pushBack ("ma");
                REQUIRE (buf.front ().first == std::string ("ala"));
        }

        {
                CircularQueue<uint8_t, 16> buf;
                uint8_t p[] = { 1, 2, 3, 4 };
                buf.pushBack (p, 4);
                REQUIRE (buf.front ().first[0] == 1);
                REQUIRE (buf.front ().first[1] == 2);
                REQUIRE (buf.front ().first[2] == 3);
                REQUIRE (buf.front ().first[3] == 4);
                REQUIRE (buf.front ().second == 4);
        }
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push+popByte", "[ccq2]")
{
        CircularQueue<uint8_t, 16> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2, 3 }.data (), 3));

        /*  o       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|1|2|3| | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 1);
        REQUIRE (buf.front ().first[1] == 2);
        REQUIRE (buf.front ().first[2] == 3);
        REQUIRE (buf.front ().second == 3);
        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 4, 5 }.data (), 2));

        /*  o             i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|1|2|3|2|4|5| | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 1);
        REQUIRE (buf.front ().first[1] == 2);
        REQUIRE (buf.front ().first[2] == 3);
        REQUIRE (buf.popFront ());

        /*          o     i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | |2|4|5| | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 4);
        REQUIRE (buf.front ().first[1] == 5);
        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 6, 7, 8, 9, 9, 9, 9 }.data (), 7));

        /*          o                     i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | |2|4|5|7|6|7|8|9|9|9|9| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 4);
        REQUIRE (buf.front ().first[1] == 5);
        REQUIRE (buf.popFront ());

        /*                o               i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | |7|6|7|8|9|9|9|9| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 6);
        REQUIRE (buf.front ().first[1] == 7);
        REQUIRE (buf.front ().first[2] == 8);
        REQUIRE (buf.front ().first[3] == 9);
        REQUIRE (buf.front ().first[4] == 9);
        REQUIRE (buf.front ().first[5] == 9);
        REQUIRE (buf.front ().first[6] == 9);
        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 2, 1 }.data (), 3));

        /*          i     o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|3|2|1| | | |7|6|7|8|9|9|9|9|E|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 6);
        REQUIRE (buf.front ().first[1] == 7);
        REQUIRE (buf.front ().first[2] == 8);
        REQUIRE (buf.front ().first[3] == 9);
        REQUIRE (buf.front ().first[4] == 9);
        REQUIRE (buf.front ().first[5] == 9);
        REQUIRE (buf.front ().first[6] == 9);
        REQUIRE (buf.popFront ());

        /*  o       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|3|2|1| | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.front ().first[0] == 3);
        REQUIRE (buf.front ().first[1] == 2);
        REQUIRE (buf.front ().first[2] == 1);
        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 6, 5, 4 }.data (), 3));

        /*  o               i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|3|2|1|3|6|5|4| | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 3);
        REQUIRE (buf.front ().first[1] == 2);
        REQUIRE (buf.front ().first[2] == 1);

        REQUIRE (!buf.pushBack (std::vector<uint8_t>{ 8, 8, 8, 8, 8, 8, 8, 8 }.data (), 8));
        REQUIRE (buf.pushBack (std::vector<uint8_t>{ 6, 6, 6, 6, 6, 6, 6 }.data (), 7));

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |3|3|2|1|3|6|5|4|7|6|6|6|6|6|6|6|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 3);
        REQUIRE (buf.front ().first[1] == 2);
        REQUIRE (buf.front ().first[2] == 1);
        REQUIRE (!buf.pushBack (std::vector<uint8_t>{ 1 }.data (), 1));
        REQUIRE (buf.popFront ());

        /* i        o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | |3|6|5|4|7|6|6|6|6|6|6|6|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 6);
        REQUIRE (buf.front ().first[1] == 5);
        REQUIRE (buf.front ().first[2] == 4);
        REQUIRE (buf.popFront ());

        /* i                o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | |7|6|6|6|6|6|6|6|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first[0] == 6);
        REQUIRE (buf.front ().first[1] == 6);
        REQUIRE (buf.front ().first[2] == 6);
        REQUIRE (buf.front ().first[3] == 6);
        REQUIRE (buf.front ().first[4] == 6);
        REQUIRE (buf.front ().first[5] == 6);
        REQUIRE (buf.front ().first[6] == 6);
        REQUIRE (buf.popFront ());
        REQUIRE (!buf.popFront ());
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push+popByte2", "[ccq2]")
{
        {
                CircularQueue<uint8_t, 8> buf;

                /* io
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 * | | | | | | | | | | | | | | | | |
                 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 */

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6 }.data (), 6));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.getSize () == 0);
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2, 3, 4 }.data (), 4));
        }

        {
                // Błąd numer 1, ale nie ten którego szukam
                CircularQueue<uint8_t, 8> buf;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2 }.data (), 2));

                /*  o     i
                 * +-+-+-+-+-+-+-+-+
                 * |2|1|2| | | | | |
                 * +-+-+-+-+-+-+-+-+
                 */

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4 }.data (), 2));

                /*  o           i
                 * +-+-+-+-+-+-+-+-+
                 * |2|1|2|2|3|4| | |
                 * +-+-+-+-+-+-+-+-+
                 */

                REQUIRE (buf.getSize () == 2);
                REQUIRE (buf.popFront ());

                /*        o     i
                 * +-+-+-+-+-+-+-+-+
                 * | | | |2|3|4| | |
                 * +-+-+-+-+-+-+-+-+
                 */

                REQUIRE (buf.getSize () == 1);
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 5, 6 }.data (), 2));
        }

        {
                // Błąd numer 2, ktróry się ujawnił na urządzeniu.
                CircularQueue<uint8_t, 8> buf;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2 }.data (), 2));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4 }.data (), 2));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 5, 6 }.data (), 2));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 7, 8 }.data (), 2));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.getSize () == 0);

                /*
                 * To już nie przechodziło, mimo że w kolekcji buf liczba elementów była równa zero.
                 * Problem chyba polegał na tym, że w popFront był (jest) warunek, który sprawdzał czy
                 * *output == EOB, a tam były jakieś śmieci z poprzednich elementów.
                 */
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 9, 0 }.data (), 2));
        }

        {
                // Błąd numer 2 test 2
                CircularQueue<uint8_t, 8> buf;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }.data (), 7));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4, 5, 6, 7 }.data (), 5));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4 }.data (), 2));
        }

        {
                // Błąd numer 2 test 3
                CircularQueue<uint8_t, 8> buf;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }.data (), 7));
                REQUIRE (buf.popFront ());

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1 }.data (), 1));
                REQUIRE (buf.getSize () == 1);
                REQUIRE (buf.front ().first[0] == 1);

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 2 }.data (), 1));
                REQUIRE (buf.getSize () == 2);

                REQUIRE (buf.popFront ());
                REQUIRE (buf.front ().first[0] == 2);
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4 }.data (), 2));

                REQUIRE (buf.getSize () == 2);
                REQUIRE (buf.popFront ());
                REQUIRE (buf.front ().first[0] == 3);
                REQUIRE (buf.front ().first[1] == 4);
        }

        {
                // Błąd numer 2 test 3
                CircularQueue<uint8_t, 8> buf;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }.data (), 7));
                REQUIRE (buf.popFront ());

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1 }.data (), 1));
                REQUIRE (buf.getSize () == 1);
                REQUIRE (buf.front ().first[0] == 1);

                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 2 }.data (), 1));
                REQUIRE (buf.getSize () == 2);

                REQUIRE (buf.popFront ());
                REQUIRE (buf.front ().first[0] == 2);
                REQUIRE (buf.popFront ());
                REQUIRE (buf.isEmpty ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 3, 4 }.data (), 2));

                REQUIRE (buf.getSize () == 1);
                REQUIRE (buf.front ().first[0] == 3);
                REQUIRE (buf.front ().first[1] == 4);
                REQUIRE (buf.popFront ());
                REQUIRE (buf.isEmpty ());
        }

        {
                /*
                 * Błąd numer 3 - polegał na tym, że jak się dodało element zajmujący MAX_SIZE (7+1), to
                 * w popFront porównanie z EOB testowało bajt wychodzacy poza obszar pamięci kolekcji.
                 */
                uint8_t *p = new uint8_t[128];
                std::fill (p, p + sizeof (p), 0xff);
                CircularQueue<uint8_t, 8> *cp = new (p) CircularQueue<uint8_t, 8>;
                CircularQueue<uint8_t, 8> &buf = *cp;
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 1, 2, 3, 4, 5 }.data (), 5));
                REQUIRE (buf.popFront ());
                REQUIRE (buf.pushBack (std::vector<uint8_t>{ 2, 3, 4, 5, 6 }.data (), 5));
                delete [] p;
        }
}
