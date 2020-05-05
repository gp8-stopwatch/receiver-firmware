/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef FLASH_EEPROM_STORAGE_H
#define FLASH_EEPROM_STORAGE_H

#include "ErrorHandler.h"
#include "Hal.h"
#include "ICircullarQueueStorage.h"
#include "IRandomAccessStorage.h"
#include <cstring>

/**
 * @brief Example implenentation of IStorege interface using files, but emulating
 * flash which emulates eeprom :D Remarks : although this class has EEPROM like interface
 * (it was meant to implement eeprom emulated in flash) it is most efficient (and thus
 * recommended) to cache your record (of size 'capacity') in RAM, and store it as a whole
 * using store (data, "capacity_bytes", 0). Remember, that every time you invoke store (even
 * with length == 1) you actually append whole record (of size capacity + WRITE_SIZE) to
 * flash.
 * @param PAGE_SIZE Size of a flash page in bytes.
 * @param WRITE_SIZE Number of bytes that can be simultaneously written to the flash.
 */
template <size_t PAGE_SIZE, size_t WRITE_SIZE = 2> class FlashEepromStorage : public IRandomAccessStorage, public ICircullarQueueStorage {
public:
        /**
         * @brief Constructor
         * @param capacity Size of a logical record which will be stored. For EEPROM memories it would be
         * the memory size, but since we emulate EEPROM in FLASH, we need to know how much of the usable
         * memory will be implented in much bigger region of FLASH. Then, every time you change at least
         * 1 word (WRITE_SIZE) of that memory, whole new record will be append to the FLASH.
         * @param numOfPages
         * @param address
         */
        FlashEepromStorage (size_t capacity, size_t numOfPages, size_t address);
        virtual ~FlashEepromStorage ();

        virtual void init ();

        /// See constructor for exmplanation.
        virtual size_t getCapacity () const { return capacity; }

        /**
         * Remarks : for flash, the most efficient scenario is when length == capacity. If length <
         * capacity, then this implementation would read prevoius record, modify it, and store as a
         * whole.
         */

        /**
         * @param offset Where to save the data (in bytes counted from the start of the record). Remember that the record
         * has a length of 'capacity' bytes. Do not confuse offset with flash memory address. It is less than that.
         */
        virtual void store (uint8_t *data, size_t length, size_t offset);
        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset, size_t history);
        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset) { return read (out, length, offset, 0); }
        virtual void clear ();

        // For debugging purposes. Not an API.
        uint8_t *_getContents () { return contents; }
        // For debugging purposes. Not an API.
        size_t _getCurrentPage () const { return currentPage; }
        // For debugging purposes. Not an API.
        size_t _getCurrentOffset () const { return currentOffset; }

protected:
        /// Clear one page.
        virtual void clearPage (size_t address);
        /// Store 1 word (WRITE_SIZE bytes long).
        virtual void storeWordImpl (uint8_t const *word, size_t address);

        void initOffsets ();

private:
        /// Do the book keeping after a single word was stored.
        void increaseAndClear ();
        void storeImpl (uint8_t *data, size_t length, size_t offset);

        /**
         * @brief Store one word of size WRITE_SIZE, and increase currentPage and currentOffset accordingly.
         * If page boundary is exceeded, new page will be used, and if last page is full, first page will be
         * cleared and used and so on.
         */
        void storeWord (uint8_t const *word);

protected:
        size_t capacity;
        size_t numOfPages;
        size_t address;

        /// Number of page which will receive data upon next write.
        size_t currentPage;
        /// Offset ion current page which will receive data upon next write.
        size_t currentOffset;

        static uint8_t END_MARKER[WRITE_SIZE];
        uint8_t *contents;
};

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
uint8_t FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::END_MARKER[WRITE_SIZE] = {
        0,
};

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::FlashEepromStorage (size_t capacity, size_t numOfPages, size_t address)
    : capacity (capacity), numOfPages (numOfPages), address (address)
{
        if (capacity % WRITE_SIZE != 0) {
                Error_Handler ();
        }

        // If capacity is to big, we cannot properly calculate where is the end of stored data (the gap is to smal).
        if (capacity + WRITE_SIZE >= PAGE_SIZE / 2) {
                Error_Handler ();
        }

        //        // Init END_MARKER
        //        for (size_t i = 0; i < WRITE_SIZE; ++i) {
        //                END_MARKER[i] = 0xff;
        //        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::init ()
{
        contents = (uint8_t *)address;

        /*---------------------------------------------------------------------------*/

        initOffsets ();
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::initOffsets ()
{
        if (!contents) {
                Error_Handler ();
        }

        /*
         * Read all the data, ad find currentPage and currentOffset. If those two variables cannot be determined,
         * clear all pages, and start from scratch (last sentence not implemented).
         */
        currentOffset = 0;
        currentPage = 0;

        for (size_t i = capacity; i < PAGE_SIZE * numOfPages; i += capacity + WRITE_SIZE) {
                if (memcmp (END_MARKER, contents + i, WRITE_SIZE) != 0) {
                        break;
                }

                currentOffset += capacity + WRITE_SIZE;

                if (currentOffset >= PAGE_SIZE) {
                        ++currentPage;
                        currentOffset %= PAGE_SIZE;
                }
        }

        if (currentPage >= numOfPages) {
                clear ();
                currentOffset = 0;
                currentPage = 0;
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::~FlashEepromStorage () {}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::store (uint8_t *data, size_t length, size_t offset)
{
        if (offset % WRITE_SIZE != 0 || length % WRITE_SIZE != 0) {
                Error_Handler ();
        }

        // Capacity boundary exceeded.
        if (length + offset > capacity) {
                Error_Handler ();
        }

        if (length < capacity) {
                uint8_t tmp[capacity + WRITE_SIZE];
                // Read current state of the data.
                read (tmp, capacity, 0);
                // Modify whats needed, and store as usual.
                memcpy (tmp + offset, data, length);
                storeImpl (tmp, length, offset);
        }
        else {
                storeImpl (data, length, offset);
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeImpl (uint8_t *d, size_t length, size_t offset)
{
        // We are on the last page, and this write will exceed it
        if (currentPage == numOfPages - 1 && currentOffset + capacity + WRITE_SIZE > PAGE_SIZE) {
                currentPage = 0;
                currentOffset = 0;
                clearPage (address);
        }

        // Store capacity bytes of data alngside with end-marker.
        for (size_t i = 0; i < capacity / WRITE_SIZE; ++i) {
                storeWord (d + i * WRITE_SIZE);
        }

        storeWord (END_MARKER);
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeWord (uint8_t const *word)
{
        size_t address = this->address + currentPage * PAGE_SIZE + currentOffset;
        storeWordImpl (word, address);
        increaseAndClear ();
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeWordImpl (uint8_t const *word, size_t address)
{
#ifndef UNIT_TEST
        HAL_FLASH_Unlock ();

        if (HAL_FLASH_Program (FLASH_TYPEPROGRAM_HALFWORD, address, *reinterpret_cast<uint16_t const *> (word)) != HAL_OK) {
                Error_Handler ();
        }

        CLEAR_BIT (FLASH->CR, (FLASH_CR_PG));
        HAL_FLASH_Lock ();
#endif
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::increaseAndClear ()
{
        currentOffset += WRITE_SIZE;

        if (currentOffset >= PAGE_SIZE) {

                currentOffset = 0;
                ++currentPage;

                if (currentPage >= numOfPages) {
                        currentPage = 0;
                }

                clearPage (this->address + currentPage * PAGE_SIZE);
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
uint8_t const *FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::read (uint8_t *out, size_t length, size_t offset, size_t history)
{
        if (offset + length > capacity) {
                Error_Handler ();
        }

        int availableMemory = PAGE_SIZE * numOfPages;
        int remainingUnusedMemoryAtTheEnd = (availableMemory) % (capacity + WRITE_SIZE);

        int prevOffset = currentOffset;
        int prevPage = currentPage;
        int start = int((capacity + WRITE_SIZE) * (history + 1) - offset);

        if (prevOffset - start >= 0) {
                prevOffset -= start;
        }
        else if (prevPage - 1 >= 0) {
                --prevPage;
                prevOffset -= start;
                prevOffset += PAGE_SIZE;
        }
        else {
                start -= prevOffset;
                prevOffset = availableMemory - remainingUnusedMemoryAtTheEnd - start;

                prevPage = prevOffset / PAGE_SIZE;
                prevOffset = prevOffset % PAGE_SIZE;
        }

        uint8_t *ret = contents + prevPage * PAGE_SIZE + prevOffset + offset;
        if (out) {
                memcpy (out, ret, length);
        }

        return ret;
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::clear ()
{
        for (size_t i = 0; i < numOfPages; ++i) {
                clearPage (address + i * PAGE_SIZE);
        }

        currentOffset = currentPage = 0;
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::clearPage (size_t address)
{
#ifndef UNIT_TEST
        HAL_FLASH_Unlock ();

        FLASH_EraseInitTypeDef er;
        er.PageAddress = address;
        er.TypeErase = FLASH_TYPEERASE_PAGES;
        er.NbPages = 1;

        uint32_t per;

        if (HAL_FLASHEx_Erase (&er, &per) != HAL_OK) {
                Error_Handler ();
        }

        CLEAR_BIT (FLASH->CR, (FLASH_CR_PER));
        HAL_FLASH_Lock ();
#endif
}

#endif // FILERANDOMACCESSSTORAGE_H
