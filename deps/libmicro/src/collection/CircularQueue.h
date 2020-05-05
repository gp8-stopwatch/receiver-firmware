/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_CIRCULARQUEUE_H
#define LIB_MICRO_CIRCULARQUEUE_H

#include <ErrorHandler.h>
#include <cstdint>
#include <cstring>
#include <utility>

/**
 * Circular queue which was designed for for storing zero-ended strings which character type is T.
 * It allows zero-copy reads when T == char. It was unit tested with T == char and T == uint8_t.
 *
 * Usage example:
 *
 * CharacterCircularQueue<128> buf;
 *   buf.pushBack ("quick"));
 *   buf.pushBack ("brown"));
 *   buf.pushBack ("fox"));
 *
 *   REQUIRE (buf.front ().first == std::string ("quick"));
 *
 *   buf.popFront ();
 *   REQUIRE (buf.front ().first == std::string ("brown"));
 *
 *   buf.popFront ();
 *   REQUIRE (buf.front ().first == std::string ("fox"));
 *
 * CircularQueue <uint8_t, 128> buf2;
 * buf.pushBack (std::vector<uint8_t>{ 1, 2, 3 }.data (), 3);
 *
 * Description:
 *
 *
 * Limitations:
 * - Elements (i.e. strings you push onto the stack) can be only EOB-1 long,
 *   which currently equals 254 bytes (with '\0'). Of course if MAX_SIZE must be
 *   taken into account.
 * - Number of elements (strings) you can push is limited to 255.
 *
 * Invariants:
 * - input always points to the empty space in buffer, where new data goes.
 * - output always points to the oldest chunk of data (data to be read first).
 *
 * Disclaimer:
 * This collection is not intended to implement STL like behabiour.  It has very
 * limited API, and was created for specified purpose. Thus method names were
 * written in camel case to clearly show this.
 */
template <typename T, size_t MAX_SIZE> class CircularQueueBase {
public:
        /// End Of Buffer
        enum { EOB = 0xff };
        using ElementType = T;
        using SizeType = uint8_t;

        CircularQueueBase () : input (buffer), output (buffer), elementsNo (0) { memset (buffer, 0x00, MAX_SIZE); }

        /**
         * Pushes an element onto the queue. If no space is available, it returns false.
         */
        bool pushBack (ElementType const *s, SizeType size);

        /**
         * Returns first (the oldest) element of the queue and its length. Warning : if collection
         * is empty, this method returns garbage. It is up to the user to check beforehand.
         * Warning 2 : second member of returned pair contains the length of returned element.
         * Warning 3 : If T == char, then remember, that second contains length WITH '\0'.
         */
        std::pair<ElementType const *, SizeType> front () const;

        /**
         * Removes the oldest element if any. If no elements are available for removing,
         * returns false.
         */
        bool popFront ();

        /**
         * Returns number of elements pushed back. Warning : it does not tell you the
         * number of bytes used to store your elemenets. Only the elements number.
         */
        size_t getSize () const { return elementsNo; }

        /// Returns if the collection is empty.
        bool isEmpty () const { return getSize () == 0; }

protected:
        ElementType buffer[MAX_SIZE];
        ElementType *input;
        ElementType *output;
        SizeType elementsNo;
};

/*****************************************************************************/

template <typename T, size_t MAX_SIZE> bool CircularQueueBase<T, MAX_SIZE>::pushBack (ElementType const *s, SizeType len)
{
        if (len > EOB - 1) {
                Error_Handler ();
        }

        ElementType *dest;
        ElementType *dataEnd = (output <= input) ? (buffer + MAX_SIZE) : (output);

        // Special case where every byte of 'buffer' is used
        if (input == output && elementsNo) {
                dataEnd = output;
        }

        // Is there free space at the end?
        if (input + len + 1 < dataEnd) {
                dest = input;
                input += len + 1;
        }
        else if (input + len + 1 == dataEnd) {
                dest = input;
                input = buffer;
        }
        // Is here free space on the beginnig?
        else if (buffer + len + 1 <= output) {
                *input = EOB;
                dest = buffer;
                input = buffer + len + 1;
        }
        else {
                return false;
        }

        // Another special case, when collection is empty, but output points to garbage
        if (!elementsNo) {
                output = dest;
        }

        *dest = len;
        memcpy (dest + 1, s, len);
        ++elementsNo;
        return true;
}

/*****************************************************************************/

template <typename T, size_t MAX_SIZE>
std::pair<typename CircularQueueBase<T, MAX_SIZE>::ElementType const *, typename CircularQueueBase<T, MAX_SIZE>::SizeType>
CircularQueueBase<T, MAX_SIZE>::front () const
{
        return std::make_pair<ElementType const *, SizeType> (output + 1, static_cast<SizeType> (*output));
}

/*****************************************************************************/

template <typename T, size_t MAX_SIZE> bool CircularQueueBase<T, MAX_SIZE>::popFront ()
{
        if (!elementsNo) {
                return false;
        }

        --elementsNo;

        if (elementsNo == 0) {
                input = output = buffer;
                return true;
        }

        output += *output + 1;

        /*
         * This loks like a bug, because *output can point to memory past the buffer, but
         * if so, then first condition should also be true, and the second would not even
         * be run.
         */
        if (output >= buffer + MAX_SIZE || *output == ElementType (EOB)) {
                output = buffer;
        }

        return true;
}

/*****************************************************************************/
/* CircularQueue                                                             */
/*****************************************************************************/

/**
 *
 */
template <typename T, size_t MAX_SIZE> class CircularQueue : public CircularQueueBase<T, MAX_SIZE> {
};

/*****************************************************************************/
/* CircularQueue <char>                                                      */
/*****************************************************************************/

/**
 * General implementation. @see CircularQueueBase.
 */
template <size_t MAX_SIZE> class CircularQueue<char, MAX_SIZE> : public CircularQueueBase<char, MAX_SIZE> {
public:
        using ElementType = typename CircularQueueBase<char, MAX_SIZE>::ElementType;
        using SizeType = typename CircularQueueBase<char, MAX_SIZE>::SizeType;

        std::pair<ElementType const *, SizeType> front () const;
        bool pushBack (char const *s) { return CircularQueueBase<char, MAX_SIZE>::pushBack (s, strlen (s) + 1); }
};

/*****************************************************************************/

/**
 * Partial specialization for char.
 */
template <size_t MAX_SIZE>
std::pair<typename CircularQueue<char, MAX_SIZE>::ElementType const *, typename CircularQueue<char, MAX_SIZE>::SizeType>
CircularQueue<char, MAX_SIZE>::front () const
{
        return std::make_pair<ElementType const *, SizeType> (CircularQueueBase<char, MAX_SIZE>::output + 1,
                                                              static_cast<SizeType> (*CircularQueueBase<char, MAX_SIZE>::output) - 1);
}

/*****************************************************************************/

template <size_t MAX_SIZE> using CharacterCircularQueue = CircularQueue<char, MAX_SIZE>;

#endif // CHARACTERCIRCULARQUEUE_H
