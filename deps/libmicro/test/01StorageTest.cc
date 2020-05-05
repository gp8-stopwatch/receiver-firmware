/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include <ErrorHandler.h>
#include <storage/FileRandomAccessStorage.h>

uint8_t const comp[]
        = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x00, 0x00,
            0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00,
            0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00 };

/**
 * @brief Storing some (little amounts of) data to freshly cleared file.
 */
TEST_CASE ("StorageHelloWorld", "[storage]")
{
        {
                // Basic : only single store operation.
                FileRandomAccessStorage<2048> s (8, 4, 0);
                s.init ();
                s.clear ();
                REQUIRE (s.getCapacity () == 8);

                uint8_t data1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
                s.store (data1, 8, 0);

                REQUIRE (memcmp (s._getContents (), comp, 10) == 0);
                REQUIRE (s._getCurrentPage () == 0);
        }

        {
                // 2 stores. Second should be appended ster the first alongside with END_MARKER.
                FileRandomAccessStorage<2048> s (8, 4, 0);
                s.init ();
                s.clear ();
                REQUIRE (s.getCapacity () == 8);

                uint8_t data1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
                s.store (data1, 8, 0);
                uint8_t data2[8] = { 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
                s.store (data2, 8, 0);

                REQUIRE (memcmp (s._getContents (), comp, 20) == 0);
                REQUIRE (s._getCurrentPage () == 0);
        }
}

/**
 * @brief Storing data which exceeds page boundary. PAGE_SIZE is divisible by Record size (capacity) + END_MARKER size.
 */
TEST_CASE ("StorageExceeded", "[storage]")
{
        {
                // 2 stores. Second should be appended ster the first alongside with END_MARKER.
                FileRandomAccessStorage<40> s (8, 4, 0);
                s.init ();
                s.clear ();
                REQUIRE (s.getCapacity () == 8);

                uint8_t data1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
                s.store (data1, 8, 0);

                uint8_t data2[8] = { 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
                s.store (data2, 8, 0);

                uint8_t data3[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
                s.store (data3, 8, 0);

                REQUIRE (s._getCurrentPage () == 0);

                uint8_t data4[8] = { 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20 };
                s.store (data4, 8, 0);

                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (memcmp (s._getContents (), comp, 40) == 0);

                /*---------------------------------------------------------------------------*/

                s.store (data4, 8, 0);
                s.store (data3, 8, 0);
                s.store (data2, 8, 0);
                REQUIRE (s._getCurrentPage () == 1);

                s.store (data1, 8, 0);
                REQUIRE (s._getCurrentPage () == 2);

                REQUIRE (memcmp (s._getContents (), comp, 80) == 0);
        }
}

/**
 * @brief Storing data which exceeds page boundary. PAGE_SIZE is NOT divisible by Record size (capacity) + END_MARKER size.
 */
TEST_CASE ("StorageExceeded2", "[storage]")
{
        {
                // 2 stores. Second should be appended ster the first alongside with END_MARKER.
                FileRandomAccessStorage<32> s (8, 4, 0);
                s.init ();
                s.clear ();
                REQUIRE (s.getCapacity () == 8);

                uint8_t data1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
                s.store (data1, 8, 0);
                REQUIRE (s._getCurrentPage () == 0);

                uint8_t data2[8] = { 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
                s.store (data2, 8, 0);
                REQUIRE (s._getCurrentPage () == 0);

                uint8_t data3[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
                s.store (data3, 8, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 30);

                uint8_t const comp2[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                                          0x0f, 0x10, 0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, 0xff, 0xff };

                REQUIRE (memcmp (s._getContents (), comp2, 32) == 0);

                /*---------------------------------------------------------------------------*/

                s.store (data3, 8, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 8);

                s.store (data3, 8, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 18);

                s.store (data2, 8, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 28);

                s.store (data1, 8, 0);
                REQUIRE (s._getCurrentPage () == 2);
                REQUIRE (s._getCurrentOffset () == 6);

                uint8_t const comp3[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                          0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                                          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //
                                          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //
                                          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //
                                          0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                                          0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00 };

                REQUIRE (memcmp (s._getContents (), comp3, 70) == 0);
        }
}

/**
 * @brief Storing so much data, that it wraps around, and starts from the begining.
 */
TEST_CASE ("StoreWholeFileExceeded", "[storage]")
{
        FileRandomAccessStorage<32> s (8, 3, 0);
        s.init ();
        s.clear ();
        REQUIRE (s.getCapacity () == 8);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 0);

        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 10);

        uint8_t data2[8] = { 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 20);

        uint8_t data3[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 30);

        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 8);

        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 18);

        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 28);

        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 6);

        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 16);

        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 26);

        uint8_t const comp3[] = {
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //

                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //

                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //

                0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };

        REQUIRE (memcmp (s._getContents (), comp3, 96) == 0);

        /*
         * Now we will exceed the PAGE_SIZE * numOfPages limit, so FileRandomAccessStorage
         * will start saving from the begining
         */
        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 10);

        uint8_t const comp4[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //

                                  0xff, 0xff, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                                  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //

                                  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x00, 0x00, //
                                  0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, //

                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

        REQUIRE (memcmp (s._getContents (), comp4, 96) == 0);

        uint8_t const *p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("FirstRead, not aligned", "[storage]")
{
        FileRandomAccessStorage<32> s (8, 3, 0);
        s.init ();
        s.clear ();

        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 10);

        uint8_t out[128];
        uint8_t const *p = s.read (out, 8, 0);

        REQUIRE (memcmp (p, data1, 8) == 0);
        REQUIRE (memcmp (out, data1, 8) == 0);

        /*---------------------------------------------------------------------------*/

        uint8_t data2[8] = { 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 20);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data2, 8) == 0);
        REQUIRE (memcmp (out, data2, 8) == 0);

        /*---------------------------------------------------------------------------*/

        uint8_t data3[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 30);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data3, 8) == 0);
        REQUIRE (memcmp (out, data3, 8) == 0);

        /*---------------------------------------------------------------------------*/

        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 8);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);
        REQUIRE (memcmp (out, data1, 8) == 0);

        /*---------------------------------------------------------------------------*/

        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 18);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data2, 8) == 0);
        REQUIRE (memcmp (out, data2, 8) == 0);
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("FirstRead aligned", "[storage]")
{
        FileRandomAccessStorage<30> s (8, 3, 0);
        s.init ();
        s.clear ();

        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 10);

        uint8_t out[128];
        uint8_t const *p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);
        REQUIRE (memcmp (out, data1, 8) == 0);

        /*---------------------------------------------------------------------------*/

        uint8_t data2[8] = { 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 20);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data2, 8) == 0);
        REQUIRE (memcmp (out, data2, 8) == 0);

        /*---------------------------------------------------------------------------*/

        uint8_t data3[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };
        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 0);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data3, 8) == 0);
        REQUIRE (memcmp (out, data3, 8) == 0);

        /*---------------------------------------------------------------------------*/

        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 10);

        p = s.read (out, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);
        REQUIRE (memcmp (out, data1, 8) == 0);

        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 20);
        p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data2, 8) == 0);

        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 0);
        p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data3, 8) == 0);

        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 10);
        p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);

        s.store (data2, 8, 0);
        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 20);
        p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data2, 8) == 0);

        s.store (data3, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 0);
        p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data3, 8) == 0);
}

/**
 * @brief Storing less than capacity
 */
TEST_CASE ("StorageLessThanCapacity", "[storage]")
{
        // 2 stores. Second should be appended ster the first alongside with END_MARKER.
        FileRandomAccessStorage<40> s (8, 4, 0);
        s.init ();
        s.clear ();
        REQUIRE (s.getCapacity () == 8);

        uint8_t data1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        s.store (data1, 8, 0);
        uint8_t const *p = s.read (nullptr, 8, 0);
        REQUIRE (memcmp (p, data1, 8) == 0);

        uint8_t data2[] = { 0xa, 0xb };
        s.store (data2, 2, 2);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 1, 2, 0xa, 0xb, 5, 6, 7, 8 };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        s.store (data2, 2, 6);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 1, 2, 0xa, 0xb, 5, 6, 0xa, 0xb };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        uint8_t data3[] = { 0xc, 0xd };
        s.store (data3, 2, 0);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0xc, 0xd, 0xa, 0xb, 5, 6, 0xa, 0xb };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 0);

        uint8_t const comp4[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                  0x01, 0x02, 0x0a, 0x0b, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                                  0x01, 0x02, 0x0a, 0x0b, 0x05, 0x06, 0x0a, 0x0b, 0x00, 0x00, //
                                  0x0c, 0x0d, 0x0a, 0x0b, 0x05, 0x06, 0x0a, 0x0b, 0x00, 0x00, //
                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

        REQUIRE (memcmp (s._getContents (), comp4, 40) == 0);

        /*---------------------------------------------------------------------------*/

        s.store (data3, 2, 2);
        s.store (data3, 2, 4);
        s.store (data3, 2, 6);
        s.store (data1, 2, 0);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x0d, 0xff };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        REQUIRE (s._getCurrentPage () == 2);
        REQUIRE (s._getCurrentOffset () == 0);

        uint8_t const comp5[] = {
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                0x01, 0x02, 0x0a, 0x0b, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, //
                0x01, 0x02, 0x0a, 0x0b, 0x05, 0x06, 0x0a, 0x0b, 0x00, 0x00, //
                0x0c, 0x0d, 0x0a, 0x0b, 0x05, 0x06, 0x0a, 0x0b, 0x00, 0x00, //

                0x0c, 0x0d, 0x0c, 0x0d, 0x05, 0x06, 0x0a, 0x0b, 0x00, 0x00, //
                0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x0d, 0x0a, 0x0b, 0x00, 0x00, //
                0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x0d, 0x00, 0x00, //
                0x01, 0x02, 0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x0d, 0x00, 0x00, //
        };

        REQUIRE (memcmp (s._getContents (), comp5, 80) == 0);
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("StorageLessThanCapacity unaligned", "[storage]")
{

        FileRandomAccessStorage<32> s (8, 3, 0);
        s.init ();
        s.clear ();
        REQUIRE (s.getCapacity () == 8);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 0);

        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        s.store (data1, 8, 0);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 10);

        uint8_t const *p;
        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        uint8_t data2[2] = { 0x09, 0x0a };
        s.store (data2, 2, 6);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 20);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        s.store (data2, 2, 4);
        REQUIRE (s._getCurrentPage () == 0);
        REQUIRE (s._getCurrentOffset () == 30);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        s.store (data2, 2, 2);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 8);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        /*
         * Now we will exceed the PAGE_SIZE * numOfPages limit, so FileRandomAccessStorage
         * will start saving from the begining
         */
        s.store (data2, 2, 0);
        REQUIRE (s._getCurrentPage () == 1);
        REQUIRE (s._getCurrentOffset () == 18);

        {
                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("Restore unaligned", "[storage]")
{

        uint8_t const *p;
        uint8_t data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        uint8_t data2[2] = { 0x09, 0x0a };

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.clear ();
                REQUIRE (s.getCapacity () == 8);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 0);

                s.store (data1, 8, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 10);

                {
                        p = s.read (nullptr, 8, 0);
                        uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
                        REQUIRE (memcmp (p, data, 8) == 0);
                }
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 6);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 20);

                {
                        p = s.read (nullptr, 8, 0);
                        uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x09, 0x0a };
                        REQUIRE (memcmp (p, data, 8) == 0);
                }
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 4);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 30);

                {
                        p = s.read (nullptr, 8, 0);
                        uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x09, 0x0a, 0x09, 0x0a };
                        REQUIRE (memcmp (p, data, 8) == 0);
                }
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 2);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 8);

                {
                        p = s.read (nullptr, 8, 0);
                        uint8_t data[] = { 0x01, 0x02, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                        REQUIRE (memcmp (p, data, 8) == 0);
                }
        }

        /*---------------------------------------------------------------------------*/

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 18);

                {
                        p = s.read (nullptr, 8, 0);
                        uint8_t data[] = { 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                        REQUIRE (memcmp (p, data, 8) == 0);
                }
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data1, 8, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 28);
                p = s.read (nullptr, 8, 0);
                REQUIRE (memcmp (p, data1, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 0);
                REQUIRE (s._getCurrentPage () == 2);
                REQUIRE (s._getCurrentOffset () == 6);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x09, 0x0a, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 2);
                REQUIRE (s._getCurrentPage () == 2);
                REQUIRE (s._getCurrentOffset () == 16);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x09, 0x0a, 0x09, 0x0a, 0x05, 0x06, 0x07, 0x08 };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 4);
                REQUIRE (s._getCurrentPage () == 2);
                REQUIRE (s._getCurrentOffset () == 26);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a, 0x07, 0x08 };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data2, 2, 6);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 10);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data1, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 20);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }

        {
                FileRandomAccessStorage<32> s (8, 3, 0);
                s.init ();
                s.store (data1, 2, 2);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 30);

                p = s.read (nullptr, 8, 0);
                uint8_t data[] = { 0x01, 0x02, 0x01, 0x02, 0x09, 0x0a, 0x09, 0x0a };
                REQUIRE (memcmp (p, data, 8) == 0);
        }
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("Restore1Page", "[storage]")
{

        uint8_t const *p;
        uint8_t data1[] = { 0x01, 0x02 };
        uint8_t data2[] = { 0x03, 0x04 };
        uint8_t data3[] = { 0x05, 0x06 };
        uint8_t data4[] = { 0x07, 0x08 };

        {
                FileRandomAccessStorage<10> s (2, 1, 0);
                s.init ();
                s.clear ();

                s.store (data1, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data1, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 1, 0);
                s.init ();
                s.store (data2, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 8);
                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data2, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 1, 0);
                s.init ();
                s.store (data3, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);
                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data3, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 1, 0);
                s.init ();
                s.store (data4, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 8);
                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data4, 2) == 0);
        }
}

/**
 * @brief History
 */
TEST_CASE ("History1Aligned", "[storage]")
{

        uint8_t const *p;
        uint8_t data1[] = { 0x01, 0x02 };
        uint8_t data2[] = { 0x03, 0x04 };
        uint8_t data3[] = { 0x05, 0x06 };
        uint8_t data4[] = { 0x07, 0x08 };
        uint8_t data5[] = { 0x09, 0x0a };
        uint8_t data6[] = { 0x0b, 0x0c };

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.clear ();

                s.store (data1, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data1, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data2, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 8);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data2, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data1, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data3, 2, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 2);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data3, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data2, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data4, 2, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 6);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data4, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data3, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data5, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 0);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data5, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data4, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data6, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data6, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data5, 2) == 0);
        }
}

/**
 * @brief History
 */
TEST_CASE ("History1Unaligned", "[storage]")
{

        uint8_t const *p;
        uint8_t data1[] = { 0x01, 0x02 };
        uint8_t data2[] = { 0x03, 0x04 };
        uint8_t data3[] = { 0x05, 0x06 };
        uint8_t data4[] = { 0x07, 0x08 };
        uint8_t data5[] = { 0x09, 0x0a };
        uint8_t data6[] = { 0x0b, 0x0c };

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.clear ();

                s.store (data1, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data1, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data2, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 8);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data2, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data1, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data3, 2, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 2);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data3, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data2, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data4, 2, 0);
                REQUIRE (s._getCurrentPage () == 1);
                REQUIRE (s._getCurrentOffset () == 6);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data4, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data3, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data5, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 0);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data5, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data4, 2) == 0);
        }

        {
                FileRandomAccessStorage<10> s (2, 2, 0);
                s.init ();
                s.store (data6, 2, 0);
                REQUIRE (s._getCurrentPage () == 0);
                REQUIRE (s._getCurrentOffset () == 4);

                p = s.read (nullptr, 2, 0);
                REQUIRE (memcmp (p, data6, 2) == 0);

                p = s.read (nullptr, 2, 0, 1);
                REQUIRE (memcmp (p, data5, 2) == 0);
        }
}
