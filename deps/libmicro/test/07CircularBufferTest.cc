/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "collection/CircularBuffer.h"
#include <cstring>

const char *TEST_DATA_STR = "12345678901234567890";
uint8_t *TEST_DATA = (uint8_t *)TEST_DATA_STR;

/**
 * @brief TEST_CASE
 */
TEST_CASE ("Basic io buffer", "[CircularBuffer]")
{
        CircularBuffer buffer (4);

        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.maxSize () == 4);

        REQUIRE (buffer.store (TEST_DATA, 4));
        REQUIRE (buffer.size () == 4);
        REQUIRE (buffer.maxSize () == 4);
}

TEST_CASE ("IO bez zawinięcia", "[CircularBuffer]")
{

        SECTION ("dodawanie 1 raz")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 5) != 5);

                buffer.clear ();
                REQUIRE (buffer.store (TEST_DATA, 6) == 4);

                buffer.clear ();
                REQUIRE (buffer.store (TEST_DATA, 2) == 2);
                REQUIRE (buffer.size () == 2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }

        SECTION ("dodawanie 2 razy, ale bez zdjęcia, nie zapelnione do końca")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 3);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 3);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }

        SECTION ("dodawanie 2 razy, ale bez zdjęcia, całkowite zapełnienie")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 4);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (!buffer.store (TEST_DATA, 1));
        }

        SECTION ("dodawanie 2 razy, ze zdejmowaniem, nie zapełnione do końca")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareReadAll ();
                REQUIRE (buffer.size () == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 1);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }

        SECTION ("dodawanie 2 razy, ze zdejmowaniem, zapełnienie do końca.")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareReadAll ();
                REQUIRE (buffer.size () == 0);

                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareReadAll ();
                REQUIRE (buffer.size () == 0);
                REQUIRE (buffer.store (TEST_DATA, 1));
        }
}

TEST_CASE ("IO z zawinięciem", "[CircularBuffer]")
{
        SECTION ("Zawinięcie od zera.")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareReadAll ();
                REQUIRE (buffer.size () == 0);

                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 2);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                // 3 raz dodajemy
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 4);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (!buffer.store (TEST_DATA, 1));

                buffer.declareRead (3);
                REQUIRE (buffer.store (TEST_DATA, 1));
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 3);
                REQUIRE (la == 1);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 1);
        }

        SECTION ("Zawinięcie od zera, ale dodajemy po 1.")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 1);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 2);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 3);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 4);
                REQUIRE (!buffer.store (TEST_DATA, 1));

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareReadAll ();
                REQUIRE (buffer.size () == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 1);

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 2);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 3);
                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 4);
                REQUIRE (!buffer.store (TEST_DATA, 1));

                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareRead (1);
                REQUIRE (buffer.store (TEST_DATA, 1));
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 1);
                REQUIRE (la == 3);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 1);

                buffer.declareRead (1);
                REQUIRE (buffer.store (TEST_DATA, 1));
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 2);

                buffer.declareRead (1);
                REQUIRE (buffer.store (TEST_DATA, 1));
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 3);
                REQUIRE (la == 1);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 3);

                buffer.declareRead (1);
                REQUIRE (buffer.store (TEST_DATA, 1));
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }
}

TEST_CASE ("Częściowe declare", "[CircularBuffer]")
{
        SECTION ("Pierwsze")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareRead (1);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 1);
                REQUIRE (la == 3);

                buffer.declareRead (1);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);

                buffer.declareRead (1);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 3);
                REQUIRE (la == 1);

                buffer.declareRead (1);
                REQUIRE (buffer.size () == 0);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == nullptr);
                REQUIRE (la == 0);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 1);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.declareRead (1);
                REQUIRE (buffer.size () == 0);

                REQUIRE (buffer.store (TEST_DATA, 1));
                REQUIRE (buffer.size () == 1);
                buffer.retrieve (&pa, &la, &pb, &lb);
                REQUIRE (pa == buffer.buffer + 1);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }
}

TEST_CASE ("Clear", "[CircularBuffer]")
{
        CircularBuffer buffer (4);
        REQUIRE (buffer.store (TEST_DATA, 4));
        REQUIRE (buffer.size () == 4);
        buffer.clear ();
        REQUIRE (buffer.size () == 0);

        REQUIRE (buffer.store (TEST_DATA, 4));
        uint8_t *pa, *pb;
        size_t la, lb;
        buffer.retrieve (&pa, &la, &pb, &lb);
        REQUIRE (pa == buffer.buffer);
        REQUIRE (la == 4);
        REQUIRE (pb == nullptr);
        REQUIRE (lb == 0);
}

TEST_CASE ("Częściowe retrieve i częściowe declare", "[CircularBuffer]")
{
        SECTION ("Pierwsze")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb, 1);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 2);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 3);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 3);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 4);
                REQUIRE (pa == buffer.buffer);
                REQUIRE (la == 4);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }

        SECTION ("Z declare")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);
                buffer.declareRead (2);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb, 1);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 2);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);
        }

        SECTION ("Z zawinięciem")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);
                buffer.declareRead (2);
                REQUIRE (buffer.store (TEST_DATA, 2));
                REQUIRE (buffer.size () == 4);

                uint8_t *pa, *pb;
                size_t la, lb;
                buffer.retrieve (&pa, &la, &pb, &lb, 1);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 1);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 2);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == nullptr);
                REQUIRE (lb == 0);

                buffer.retrieve (&pa, &la, &pb, &lb, 3);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 1);

                buffer.retrieve (&pa, &la, &pb, &lb, 4);
                REQUIRE (pa == buffer.buffer + 2);
                REQUIRE (la == 2);
                REQUIRE (pb == buffer.buffer);
                REQUIRE (lb == 2);
        }
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("Remove last bez zawinięcia", "[CircularBuffer]")
{
        CircularBuffer buffer (3);

        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.maxSize () == 3);
        REQUIRE (buffer.offsetIn == 0);

        REQUIRE (buffer.store (TEST_DATA, 1));
        REQUIRE (buffer.offsetIn == 1);
        REQUIRE (buffer.size () == 1);
        REQUIRE (!buffer.removeLast (2));
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.offsetIn == 0);

        REQUIRE (buffer.store (TEST_DATA, 2));
        REQUIRE (buffer.size () == 2);
        REQUIRE (!buffer.removeLast (3));
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 1);
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.offsetIn == 0);

        REQUIRE (buffer.store (TEST_DATA, 3));
        REQUIRE (buffer.size () == 3);
        REQUIRE (!buffer.removeLast (4));
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 2);
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 1);
        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.offsetIn == 0);

        REQUIRE (buffer.store (TEST_DATA, 4) == 3);
        // REQUIRE (buffer.store (TEST_DATA, 3));
        REQUIRE (buffer.size () == 3);
        REQUIRE (buffer.removeLast (3));
        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.offsetIn == 0);
}

TEST_CASE ("Remove last z zawinięciem", "[CircularBuffer]")
{
        CircularBuffer buffer (3);

        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.maxSize () == 3);

        REQUIRE (buffer.store (TEST_DATA, 3));
        REQUIRE (buffer.size () == 3);
        buffer.declareRead (2);
        REQUIRE (buffer.size () == 1);
        // Zawinęło się.
        REQUIRE (buffer.store (TEST_DATA, 2));
        REQUIRE (buffer.size () == 3);

        REQUIRE (buffer.offsetIn == 2);
        REQUIRE (buffer.offsetOut == 2);

        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 2);
        REQUIRE (buffer.offsetIn == 1);

        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 1);
        REQUIRE (buffer.offsetIn == 0);

        REQUIRE (buffer.removeLast (1));
        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.offsetIn == 2);
        REQUIRE (buffer.offsetIn == buffer.offsetOut);
}

TEST_CASE ("Boundary", "[CircularBuffer]")
{
        CircularBuffer buffer (2 * 8);

        REQUIRE (buffer.size () == 0);
        REQUIRE (buffer.maxSize () == 2 * 8);

        REQUIRE (buffer.store (TEST_DATA, 8));
        REQUIRE (buffer.size () == 8);
        buffer.declareRead (8);
        REQUIRE (buffer.size () == 0);

        REQUIRE (buffer.store (TEST_DATA, 8));
        REQUIRE (buffer.size () == 8);
        buffer.declareRead (8);
        REQUIRE (buffer.size () == 0);

        REQUIRE (buffer.store (TEST_DATA, 8));
        REQUIRE (buffer.size () == 8);

        uint8_t *partA, *partB;
        size_t lenA, lenB;
        buffer.retrieve (&partA, &lenA, &partB, &lenB, 8);

        REQUIRE (lenA == 8);
        REQUIRE (lenB == 0);

        SECTION ("Declare to much")
        {
                CircularBuffer buffer (4);
                REQUIRE (buffer.store (TEST_DATA, 4));
                REQUIRE (buffer.size () == 4);
                REQUIRE (buffer.declareRead (2) == 2);
                REQUIRE (buffer.size () == 2);
                REQUIRE (buffer.declareRead (3) == 2);
                REQUIRE (buffer.size () == 0);
        }
}

/**
 * @brief Pozwoli zapisać więcej danych niż wynosi rozmiar bufora
 */
TEST_CASE ("Nadpisywanie", "[CircularBuffer]")
{
        CircularBuffer buffer (4, true);
        REQUIRE (buffer.store (TEST_DATA, 5) == 5);
        REQUIRE (buffer.size () == 4);

        uint8_t *pa, *pb;
        size_t la, lb;
        buffer.retrieve (&pa, &la, &pb, &lb);
        REQUIRE (pa == buffer.buffer + 1);
        REQUIRE (la == 3);
        REQUIRE (pb == buffer.buffer);
        REQUIRE (lb == 1);

        // 2345
        REQUIRE (*pa == '2');
        REQUIRE (*(pa + 1) == '3');
        REQUIRE (*(pa + 2) == '4');
        REQUIRE (*pb == '5');

        // Znów dodajemy 1 bajt

        REQUIRE (buffer.store (TEST_DATA, 1) == 1);
        REQUIRE (buffer.size () == 4);
        buffer.retrieve (&pa, &la, &pb, &lb);
        REQUIRE (*pa == '3');
        REQUIRE (*(pa + 1) == '4');
        REQUIRE (*pb == '5');
        REQUIRE (*(pb + 1) == '1');

        // A teraz znów dodaję 6
        REQUIRE (buffer.store (TEST_DATA, 6) == 6);
        REQUIRE (buffer.size () == 4);
        buffer.retrieve (&pa, &la, &pb, &lb);
        REQUIRE (pa == buffer.buffer);
        REQUIRE (la == 4);
        REQUIRE (pb == nullptr);
        REQUIRE (lb == 0);
        REQUIRE (*pa == '3');
        REQUIRE (*(pa + 1) == '4');
        REQUIRE (*(pa + 2) == '5');
        REQUIRE (*(pa + 3) == '6');
}

#ifdef WITH_GLOBAL_OFFSET_OUT
/**
 * @brief Global offset, ale bez zawinięcia.
 */
TEST_CASE ("Global offsetOut", "[CircularBuffer]")
{
        size_t globalOffsetOut = 0;
        CircularBuffer buffer (10, false, &globalOffsetOut);

        REQUIRE (buffer.store (TEST_DATA, 5) == 5);
        REQUIRE (buffer.size () == 5);
        REQUIRE (globalOffsetOut == 0);
        buffer.declareRead (4);
        REQUIRE (globalOffsetOut == 4);
        REQUIRE (buffer.store (TEST_DATA, 5) == 5);
        REQUIRE (globalOffsetOut == 4);
        buffer.declareReadAll ();
        REQUIRE (globalOffsetOut == 10);

        // Wyczyściłem, teraz będę dodwał i odczytywał, a globalOffsetOut ma się zwiększać przy każdym odczycie
        REQUIRE (buffer.store (TEST_DATA, 3) == 3);
        REQUIRE (buffer.size () == 3);
        buffer.declareReadAll ();
        REQUIRE (globalOffsetOut == 13);

        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.size () == 10);
        buffer.declareRead (6);
        REQUIRE (globalOffsetOut == 19);

        buffer.declareReadAll ();
        REQUIRE (globalOffsetOut == 23);
}

/**
 * @brief Z zawininięciem i zamazywaniem.
 */
TEST_CASE ("Global offsetOut overwrite", "[CircularBuffer]")
{
        size_t globalOffsetOut = 0;
        CircularBuffer buffer (10, true, &globalOffsetOut);

        REQUIRE (buffer.store (TEST_DATA, 5) == 5);
        REQUIRE (buffer.size () == 5);
        REQUIRE (globalOffsetOut == 0);
        buffer.declareRead (4);
        REQUIRE (globalOffsetOut == 4);
        REQUIRE (buffer.store (TEST_DATA, 5) == 5);
        REQUIRE (globalOffsetOut == 4);
        buffer.declareReadAll ();
        REQUIRE (globalOffsetOut == 10);

        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.size () == 10);
        REQUIRE (globalOffsetOut == 10);
        REQUIRE (buffer.store (TEST_DATA, 2) == 2);
        REQUIRE (buffer.size () == 10);
        REQUIRE (globalOffsetOut == 12);

        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.store (TEST_DATA, 10) == 10);
        REQUIRE (buffer.size () == 10);
        REQUIRE (globalOffsetOut == 62);
}
#endif
