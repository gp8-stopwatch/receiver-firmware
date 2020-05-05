/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICIRCULLARQUEUESTORAGE_H
#define ICIRCULLARQUEUESTORAGE_H

#include <cstddef>
#include <cstdint>

/**
 * @brief Binary dat storage with circullar API. It is meant to be implemented in FLASH mostly.
 */
struct ICircullarQueueStorage {
        virtual ~ICircullarQueueStorage () {}
        virtual size_t getCapacity () const = 0;

        /**
         * @brief store Append data to the storage.
         * @param data
         * @param length
         * @param offset
         */
        virtual void store (uint8_t *data, size_t length, size_t offset) = 0;

        /**
         *
         */
        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset, size_t history) = 0;

        /// Rermoves all the data.
        virtual void clear () = 0;
};

#endif // ICIRCULLARQUEUESTORAGE_H
