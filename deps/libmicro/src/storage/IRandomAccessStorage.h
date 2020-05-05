/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef IRANDOMACCESSSTORAGE_H
#define IRANDOMACCESSSTORAGE_H

#include <cstddef>
#include <cstdint>

/**
 * Binary data storage. Be it an eeprom, a file, or flash. Error reporting using Error_Handler
 * (infinite loop).
 */
struct IRandomAccessStorage {
        virtual ~IRandomAccessStorage () {}

        /**
         * @brief Maximum length of data record in in bytes. In case of EEPROM, it would simply mean
         * the length of the EEPROM.
         */
        virtual size_t getCapacity () const = 0;

        /**
         * @brief Store some data
         * @param data Data to dtore.
         * @param length Length in bytes.
         * @param offset Where to save the data (in bytes counted from the start of the record).
         */
        virtual void store (uint8_t *data, size_t length, size_t offset) = 0;

        /**
         * @brief Read some data.
         * @param out Pointer to memory region owned by the user which requested data will be copied to. Leave NULL if
         * you dont need to modify the data
         * @param length How much (in bytes).
         * @param offset Where to start reading.
         * @return Requested data (memory region is owned by library).
         */
        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset) = 0;

        /// Rermoves all the data.
        virtual void clear () = 0;
};

#endif // IRANDOMACCESSSTORAGE_H
