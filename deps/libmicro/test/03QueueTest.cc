/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "collection/Queue.h"
#include <cassert>
#include <cstdint>
#include <cstring>

typedef Queue<char[16]> StringQueue;
typedef Queue<int> IntQueue;

uint32_t HAL_GetTick ()
{
        static uint32_t i = 0;
        return ++i;
}

uint32_t serialNum = 12345;
int unitTestTentacle = 0;

TEST_CASE ("Prosta kolejka")
{
        StringQueue q (4);

        REQUIRE (q.size () == 0);

        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (!q.push_back ());
}

TEST_CASE ("Kolejka z liczbami")
{
        IntQueue q (4);
        REQUIRE (q.size () == 0);

        REQUIRE (q.push_back ());
        q.back () = 11;
        REQUIRE (q.size () == 1);
        REQUIRE (q.frontp == 0);
        REQUIRE (q.backp == 0);

        REQUIRE (q.push_back ());
        q.back () = 22;
        REQUIRE (q.size () == 2);
        REQUIRE (q.frontp == 0);
        REQUIRE (q.backp == 1);

        REQUIRE (q.push_back ());
        q.back () = 33;
        REQUIRE (q.size () == 3);
        REQUIRE (q.frontp == 0);
        REQUIRE (q.backp == 2);

        REQUIRE (q.push_back ());
        q.back () = 44;
        REQUIRE (q.size () == 4);
        REQUIRE (q.frontp == 0);
        REQUIRE (q.backp == 3);

        REQUIRE (!q.push_back ());
        REQUIRE (q.size () == 4);
        REQUIRE (q.frontp == 0);
        REQUIRE (q.backp == 3);

        // Zdejmujemy
        REQUIRE (q.frontp == 0);
        REQUIRE (q.front () == 11);

        REQUIRE (q.pop_front ());
        REQUIRE (q.size () == 3);
        REQUIRE (q.frontp == 1);
        REQUIRE (q.backp == 3);
        REQUIRE (q.front () == 22);

        REQUIRE (q.pop_front ());
        REQUIRE (q.size () == 2);
        REQUIRE (q.frontp == 2);
        REQUIRE (q.backp == 3);
        REQUIRE (q.front () == 33);

        REQUIRE (q.pop_front ());
        REQUIRE (q.size () == 1);
        REQUIRE (q.frontp == 3);
        REQUIRE (q.backp == 3);
        REQUIRE (q.front () == 44);

        // Skoro rozmiar jest 1, to dodajemy znów dane
        REQUIRE (q.push_back ());
        q.back () = 6;
        REQUIRE (q.size () == 2);

        REQUIRE (q.push_back ());
        q.back () = 7;
        REQUIRE (q.size () == 3);

        REQUIRE (q.push_back ());
        q.back () = 8;
        REQUIRE (q.size () == 4);
}

TEST_CASE ("Kolejka z napisami")
{
        StringQueue q (4);

        REQUIRE (q.push_back ());
        strcpy (q.back (), "ala");

        q.push_back ();
        strcpy (q.back (), "ma");

        q.push_back ();
        strcpy (q.back (), "kota");

        q.push_back ();
        strcpy (q.back (), "psa");

        q.pop_front ();
        char *str = q.front ();
        REQUIRE (strcmp (str, "ma") == 0);

        q.pop_front ();
        str = q.front ();
        REQUIRE (strcmp (str, "kota") == 0);

        q.pop_front ();
        str = q.front ();
        REQUIRE (strcmp (str, "psa") == 0);

        q.pop_front ();
        REQUIRE (q.size () == 0);
        // REQUIRE (q.front () == NULL);
        REQUIRE (!q.pop_front ());
}

#ifdef WITH_NEXT_AFTER_BACK
TEST_CASE ("Reserve")
{
        StringQueue q (4);

        REQUIRE (q.push_back ());
        strcpy (q.back (), "ala");
        REQUIRE (q.size () == 1);
        StringQueue::Element *el = q.back ();
        REQUIRE (strcmp (el, "ala") == 0);

        el = q.next_after_back ();
        strcpy (el, "franio");
        REQUIRE (q.size () == 1);
        el = q.back ();
        REQUIRE (strcmp (el, "ala") == 0);

        // Zwróci ten "zarezerwowany"
        el = q.next_after_back ();
        el = q.next_after_back ();
        el = q.next_after_back ();
        el = q.next_after_back ();
        REQUIRE (q.size () == 1);
        el = q.back ();
        REQUIRE (strcmp (el, "ala") == 0);

        q.push_back ();
        REQUIRE (q.size () == 2);
        el = q.back ();
        REQUIRE (strcmp (el, "franio") == 0);

        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (!q.next_after_back ());
}
#endif

/*
 * TODO! Dowiedzieć się, czemu ten test nie działa. Działa OK jeśli to nie jest test_case z Catch.h, tylko zwykły
 * program (dodałem zwykły program, nazywa się queue-standalone-test.cc
 */
#if 0
TEST_CASE ("Kolejka z napisami, C API")
{
        StringQueue q;
        string_queue_t cq = &q;

        /*
         * TODO!!! Zrobić dochodzenie czemu poniższe wywołanie powoduje, że cq, mimo, że kastuje się na dobry
         * adres (StringQueue *q = static_cast<StringQueue *> (queue)), to jednak wywołanie metody q już nie działa.
         * Mimo, że this też ma ten sam adres!!
         */

        bool b = string_queue_push_back (cq);
        REQUIRE (b);
        REQUIRE (q.size () == 1);
        strcpy (q.back (), "ala");

        string_queue_push_back (cq);
        strcpy (q.back (), "ma");

        string_queue_push_back (cq);
        strcpy (q.back (), "kota");

        string_queue_push_back (cq);
        strcpy (q.back (), "psa");

        // 9 bo było już 8 push_backów wcześniej
        REQUIRE (q.front ()->timestamp == 17);

        string_queue_pop_front (cq);
        REQUIRE (q.front ()->timestamp == 18);
        char *str = string_queue_front (cq);
        REQUIRE (strcmp (str, "ma") == 0);

        string_queue_pop_front (cq);
        REQUIRE (q.front ()->timestamp == 19);
        str = string_queue_front (cq);
        REQUIRE (strcmp (str, "kota") == 0);

        string_queue_pop_front (cq);
        REQUIRE (q.front ()->timestamp == 20);
        str = string_queue_front (cq);
        REQUIRE (strcmp (str, "psa") == 0);

        string_queue_pop_front (cq);
        REQUIRE (string_queue_size (cq) == 0);
        REQUIRE (string_queue_front (cq) == NULL);
        REQUIRE (!string_queue_pop_front (cq));
}
#endif

TEST_CASE ("Iteracja")
{
        StringQueue q (4);

        REQUIRE (q.push_back ());
        strcpy (q.back (), "ala");

        q.push_back ();
        strcpy (q.back (), "ma");

        q.push_back ();
        strcpy (q.back (), "kota");

        q.push_back ();
        strcpy (q.back (), "psa");

        q.pop_front ();

        REQUIRE (strcmp (q.front (), "ma") == 0);
        REQUIRE (strcmp (q.front (1), "kota") == 0);
        REQUIRE (strcmp (q.front (2), "psa") == 0);
        // REQUIRE (q.front (3));
}

TEST_CASE ("Pop front")
{
        IntQueue q (4);
        REQUIRE (q.size () == 0);

        REQUIRE (q.push_back ());
        REQUIRE (q.size () == 1);

        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (!q.push_back ());

        REQUIRE (q.pop_front ());
        REQUIRE (q.size () == 3);

        REQUIRE (q.pop_front (3));
        REQUIRE (q.size () == 0);

        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (q.push_back ());
        REQUIRE (!q.pop_front (5));
}

int sum (IntQueue const &q)
{
        int s = 0;

        for (size_t i = 0; i < q.size (); ++i) {
                s += q.front (i);
        }

        return s;
}

TEST_CASE ("Rolling integral")
{
        IntQueue q (4);
        q.push_back_roll ();
        q.back () = 1;
        REQUIRE (sum (q) == 1);

        q.push_back_roll ();
        q.back () = 2;
        REQUIRE (sum (q) == 3);

        q.push_back_roll ();
        q.back () = 3;
        REQUIRE (sum (q) == 6);

        q.push_back_roll ();
        q.back () = 4;
        REQUIRE (sum (q) == 10);

        // Now it starts rolling
        q.push_back_roll ();
        q.back () = 5;
        REQUIRE (sum (q) == 14);

        q.push_back_roll ();
        q.back () = 6;
        REQUIRE (sum (q) == 18);

        q.push_back_roll ();
        q.back () = 7;
        REQUIRE (sum (q) == 22);

        q.push_back_roll ();
        q.back () = 8;
        REQUIRE (sum (q) == 26);

        q.push_back_roll ();
        q.back () = 9;
        REQUIRE (sum (q) == 30);
}

TEST_CASE ("Non trivial constructor")
{
        Queue <std::vector <std::string>> q (4);

        REQUIRE (q.size () == 0);

        REQUIRE (q.push_back ());
        q.back () = {"ala", "ma", "kota"};

        REQUIRE (q.push_back ());
        q.back () = {"ola", "ma", "psa"};

        REQUIRE (q.push_back ());
        q.back () = {"krysia", "ma", "kurę"};

        REQUIRE (q.push_back ());
        q.back () = {"maciek", "ma", "krowę"};

        REQUIRE (!q.push_back ());
}
