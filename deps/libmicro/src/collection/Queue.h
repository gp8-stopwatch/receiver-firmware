/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SRC_SQUEUE_H_
#define SRC_SQUEUE_H_

#include "ErrorHandler.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>

/**
 * Simple queue with circular (rolling) as an option.
 * Not thread safe.
 */
template <typename El> class Queue {
public:
        typedef El Element;

        // malloc zapobiega uruchamianiu się konstruktorów. Konstruktor jest uruchamiany przy push_back.
        Queue (size_t maxSiz)
            : frontp (0),
              backp (maxSiz - 1),
              sizep (0),
              maxSizep (maxSiz) /*, queue ((Element *)malloc (sizeof (Element) * maxSizep))*/
              ,
              queue (new Element[maxSiz])
        {
        }

        ~Queue () { delete[] queue; }

        /**
         * Rozmiar kolejki.
         */
        size_t const &size () const { return sizep; }
        size_t const &maxSize () const { return maxSizep; }

        /**
         * @brief Odkłada nowy element na końcu kolejki. Jeśli nie ma miejsca, to zwraca false.
         * @return Czy się powiodło.
         */
        bool push_back ();

        void push_back_roll ();

        /**
         * @brief Zwraca ostatnio dodany element kolejki lub nullptr.
         * @return Zwraca ostatnio dodany element kolejki lub nullptr.
         */
        Element &back ();
        Element const &back () const { return const_cast<Queue *> (this)->back (); }

        /**
         * @brief Zwraca element, który będzie zwrócony jako następny po wykonaniu push_back, back. Dzięki temu można
         * używać tego elementu zanim się go odłoży na kolejkę. Zapobiega to przedwczesnemu zdjęciu z kolejki przez inny
         * "wątek".
         */
        //        Element *next_after_back ();

        bool pop_front (size_t number = 1);

        /**
         * @brief Zwraca pierwszy (domyślnie), lub n-ty (offset) za pierwszym. Jeśli offset >= size, to nullptr.
         */
        Element &front (size_t offset = 0);
        Element const &front (size_t offset = 0) const { return const_cast<Queue *> (this)->front (offset); }

        void clear ()
        {
                frontp = 0;
                backp = maxSizep - 1;
                sizep = 0;
        }

#ifndef UNIT_TEST
private:
#endif

        size_t frontp;
        size_t backp;
        size_t sizep;
        size_t maxSizep;
        Element *queue = nullptr;
};

/*---------------------------------------------------------------------------*/

template <typename El> bool Queue<El>::push_back ()
{
        //        __disable_irq ();

        if (sizep >= maxSizep) {
                //                __enable_irq ();
                return false;
        }

        backp = (backp + 1) % maxSizep;
        // queue[backp] = Element ();
        ++sizep;
        //        __enable_irq ();
        return true;
}

/*---------------------------------------------------------------------------*/

template <typename El> void Queue<El>::push_back_roll ()
{
        //        __disable_irq ();

        if (sizep >= maxSizep) {
                pop_front ();
        }

        backp = (backp + 1) % maxSizep;
        // queue[backp] = Element ();
        ++sizep;
        //        __enable_irq ();
}

/*---------------------------------------------------------------------------*/

template <typename El> bool Queue<El>::pop_front (size_t number)
{
        //        __disable_irq ();
        if (!size () || number > size ()) {
                //                __enable_irq ();
                return false;
        }

        frontp = (frontp + number) % maxSizep;
        sizep -= number;

        //        __enable_irq ();
        return true;
}

/*---------------------------------------------------------------------------*/

// template <typename El> typename Queue<El>::Element *Queue<El>::next_after_back ()
//{
//        if (sizep >= maxSizep) {
//                return nullptr;
//        }

//        return &queue[(backp + 1) % maxSizep];
//}

/*---------------------------------------------------------------------------*/

template <typename El> typename Queue<El>::Element &Queue<El>::back ()
{
        if (!size ()) {
                Error_Handler ();
        }

        return queue[backp];
}

/*---------------------------------------------------------------------------*/

template <typename El> typename Queue<El>::Element &Queue<El>::front (size_t offset)
{
        if (!size () || offset >= size ()) {
                Error_Handler ();
        }

        return queue[(frontp + offset) % maxSizep];
}

#endif /* SRC_SQUEUE_H_ */
