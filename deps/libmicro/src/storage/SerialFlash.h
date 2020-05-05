/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SERIALFLASH_H
#define SERIALFLASH_H

#include "Spi.h"
#include <cstdint>

/**
 * Serial flash class developed and tested on LE25U20AMB.
 */
class SerialFlash {
public:
        enum Command {
                READ = 0x03,
                SMALL_SECTOR_ERASE = 0xd7,
                SECTOR_ERASE = 0xd8,
                CHIP_ERASE = 0xc7,
                PAGE_PROGRAM = 0x02,
                WRITE_ENABLE = 0x06,
                WRITE_DISABLE = 0x04,
                POWER_DOWN = 0x0b9,
                STATUS_REGISTER_READ = 0x05,
                STATUS_REGISTER_WRITE = 0x01,
                READ_SILICON_ID1 = 0x9f,
                READ_SILICON_ID2 = 0xab,
                EXIT_POWER_DOWN_MODE = 0xab,
                GLOBAL_BLOCK_PROTECTION_UNLOCK = 0x98,
                CONFIGURATION_REGISTER_READ = 0x35,
                BLOCK_PROTECTION_REGISTER_READ = 0x72
        };

        SerialFlash (Spi *s) : spi (s) {}

        void read (uint32_t address, uint8_t *buf, size_t len);

        /**
         * Page program. Double check in your serial flash datasheet, but from what I can tell this method
         * programs 256B long pages. This means it can program up to 256 bytes but only If address is divisible
         * by 256. If its not, data exceeding 256 boundary will be wrapped to the beginning of a page.
         * @param address
         * @param buf
         * @param len
         */
        void write (uint32_t address, uint8_t const *buf, size_t len);

        enum StatusRegister {
                RDY_POS = 0,
                RDY = (1 << RDY_POS),
                WEN_POS = 1,
                WEN = 1 << WEN_POS,
                BP0_POS = 2,
                BP0 = 1 << BP0_POS,
                BP1_POS = 3,
                BP1 = 1 << BP1_POS,
                SRWP_POS = 7,
                SRWP = 1 << SRWP_POS
        };

        uint8_t statusRegisterRead () const;
        void statusRegisterWrite (uint8_t value);

        uint32_t readSiliconId1 () const;

        void writeEnable ();
        void writeDisable ();
        void globalBlockProtectionUnlock ();

        void chipErase ();
        void erase4k (uint32_t address) { sectorErase (address, 0x20); }
        void erase32k (uint32_t address) { sectorErase (address, 0xd8); }

        enum ConfigRegister {
                IOC_POS = 1,
                IOC = 1 << IOC_POS, /// I/O Configuration for SPI Mode. 1 = WP# and HOLD# pins disabled, 0 = WP# and HOLD# pins enabled
                BPNV_POS = 3,
                BPNV = 1 << BPNV_POS, /// Block-Protection Volatility State
                WPEN_POS = 7,
                WPEN = 1 << WPEN_POS ///  Write-Protection Pin (WP#) Enable
        };

        uint8_t configRegisterRead () const;

        typedef uint8_t BlockProtectionRegister[10];
        void blockProtectionRegisterRead (BlockProtectionRegister &reg) const;

private:
        void sectorErase (uint32_t address, uint8_t eraseInstruction);
        void waitUntilReady () const;

private:
        Spi *spi;
};

#endif // SERIALFLASH_H
