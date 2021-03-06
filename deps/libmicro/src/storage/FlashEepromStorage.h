/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "ErrorHandler.h"
#include "Hal.h"
#include "ICircullarQueueStorage.h"
#include "IRandomAccessStorage.h"
#include <array>
#include <cstring>
#include <etl/array_view.h>

/**
 * TODO There are a lot of possible improvements here.
 * - Test with various WRITE_SIZE-ss and capacity-s
 * - Get rid of magic constants. There are plenty of /2, *2, /4, *4 etc.
 * - Implementing two different interfaces at once makes this class hard to understand. Improove documentation maybe?
     The conceptual problem here is that this class implements an EPROM emulation done in flash which (as a side effect)
     has an option for viewing historical data (i.e. what was stored previously - kind of versioning). And this side
     effect is uded to implement stopwatch results history.
 * @brief Remarks : although this class has EEPROM like interface
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
        static_assert (WRITE_SIZE == 2 || WRITE_SIZE == 4 || WRITE_SIZE == 8);

        /**
         * @brief Constructor
         * @param capacity Size of a logical record which will be stored [in bytes]. For EEPROM memories it would be
         * the memory size, but since we emulate EEPROM in FLASH, we need to know how much of the usable
         * memory will be implented in much bigger region of FLASH. Then, every time you change at least
         * 1 word (WRITE_SIZE) of that memory, whole new record will be append to the FLASH.
         * @param numOfPages
         * @param address
         */
        FlashEepromStorage (size_t capacity, size_t numOfPages, size_t address);

        virtual void init ();

        /// See constructor for exmplanation.
        size_t getCapacity () const override { return capacity; }

        /**
         * Remarks : for flash, the most efficient scenario is when length == capacity. If length <
         * capacity, then this implementation would read prevoius record, modify it, and store as a
         * whole.
         */

        /**
         * @param offset Where to save the data (in bytes counted from the start of the record). Remember that the record
         * has a length of 'capacity' bytes. Do not confuse offset with flash memory address. It is less than that.
         */
        void store (uint8_t *data, size_t length, size_t offset) override;
        uint8_t const *read (uint8_t *out, size_t length, size_t offset, size_t history) override;
        uint8_t const *read (uint8_t *out, size_t length, size_t offset) override { return read (out, length, offset, 0); }
        void clear () override;

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
        virtual void storeWriteSizeImpl (uint8_t const *word, size_t address);

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
        void storeWriteSize (uint8_t const *word);

protected:
        size_t capacity;
        size_t numOfPages;
        size_t address;

        /// Number of page which will receive data upon next write.
        size_t currentPage;
        /// Offset ion current page which will receive data upon next write.
        size_t currentOffset;

        static constexpr std::array<uint8_t, WRITE_SIZE> END_MARKER{};
        uint8_t *contents;
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
        if (capacity + WRITE_SIZE > PAGE_SIZE / 2) {
                Error_Handler ();
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::init ()
{
        contents = (uint8_t *)address;

        // Figure out where is the first writable cell (of capacity size)
        initOffsets ();

        // If, by any chance current offset is at the very end of the page, we adjust it as the store method would.
        auto offsetToCheck = currentOffset;
        auto pageToCheck = currentPage;

        if (offsetToCheck + capacity + WRITE_SIZE > PAGE_SIZE) {
                ++pageToCheck;
                pageToCheck %= numOfPages;
                offsetToCheck = 0;
        }

        // This is the cell to be written contents. It should be empty (0xff)
        etl::array_view<uint8_t> cell{contents + offsetToCheck, contents + offsetToCheck + capacity};

        bool writable = true;

        // Check if all the bytes in the current cell are cleared (and thus writable). Clear == 0xff
        for (auto c : cell) {
                if (c != 0xff) {
                        writable = false;
                        break;
                }
        }

        /*
         * That is a fatal error. I encunetered this strange situation of flash memory beeing corrupted
         * quite a few times. I suspect that intensive debugging, and other faults (unrelated to the flash
         * itself) were what caused this. For example a proghram could hang somewhere where a trigger
         * event occured, and this is exaclty when the flash subsystem is trying to store a result. So
         * it could happen that a result was stored only partially this corrupting the flash.
         *
         * The only thing I can do is to clear the flash.
         */
        if (!writable) {
                clear ();
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::initOffsets ()
{
        if (contents == nullptr) {
                Error_Handler ();
        }

        /*
         * Read all the data, ad find currentPage and currentOffset. If those two variables cannot be determined,
         * clear all pages, and start from scratch (last sentence not implemented).
         */
        currentOffset = 0;
        currentPage = 0;

        for (size_t i = capacity; i < PAGE_SIZE * numOfPages; i += capacity + WRITE_SIZE) {
                if (memcmp (END_MARKER.data (), contents + i, WRITE_SIZE) != 0) { // No end marker found
                        break;
                }

                // End marker has been found
                currentOffset += capacity + WRITE_SIZE;

                if (currentOffset >= PAGE_SIZE) {
                        ++currentPage;
                        currentOffset %= PAGE_SIZE;
                }
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::store (uint8_t *data, size_t length, size_t offset)
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

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeImpl (uint8_t *d, size_t length, size_t offset)
{
        // We are on the last page, and this write will exceed it
        if (currentOffset + capacity + WRITE_SIZE > PAGE_SIZE) {
                ++currentPage;
                currentPage %= numOfPages;
                currentOffset = 0;
                clearPage (address + currentPage * PAGE_SIZE);
        }

        // Some special case that I cannot remember what was for.
        if (currentPage >= numOfPages) {
                currentPage = 0;
                currentOffset = 0;
                clearPage (address);
        }

        // Store capacity bytes of data alongside with end-marker.
        for (size_t i = 0; i < capacity / WRITE_SIZE; ++i) {
                storeWriteSize (d + i * WRITE_SIZE);
        }

        storeWriteSize (END_MARKER.data ());
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeWriteSize (uint8_t const *word)
{
        size_t address = this->address + currentPage * PAGE_SIZE + currentOffset;
        storeWriteSizeImpl (word, address);
        // increaseAndClear ();
        currentOffset += WRITE_SIZE;
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE>
void FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::storeWriteSizeImpl (uint8_t const *word, size_t address)
{
#ifndef UNIT_TEST
        if (HAL_FLASH_Unlock () != HAL_OK) {
                Error_Handler ();
        }

        HAL_StatusTypeDef status{};

        while ((status = FLASH_WaitForLastOperation (1000)) == HAL_TIMEOUT) {
        }

        if constexpr (WRITE_SIZE == 2) {
                status = HAL_FLASH_Program (FLASH_TYPEPROGRAM_HALFWORD, address, *reinterpret_cast<uint16_t const *> (word));
        }
        if constexpr (WRITE_SIZE == 4) {
                status = HAL_FLASH_Program (FLASH_TYPEPROGRAM_WORD, address, *reinterpret_cast<uint32_t const *> (word));
        }
        if constexpr (WRITE_SIZE == 8) {
                status = HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, address, *reinterpret_cast<uint64_t const *> (word));
        }

        if (status != HAL_OK) {
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
        int start = int ((capacity + WRITE_SIZE) * (history + 1) - offset);

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
        if (out != nullptr) {
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
