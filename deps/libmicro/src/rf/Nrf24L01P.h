/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef NRF24L0P_H
#define NRF24L0P_H

#include "Gpio.h"
#include "Spi.h"

/**
 * IRQ callback. Remember, this gets called in IRQ handler!
 */
class Nrf24L01PCallback {
public:
        virtual ~Nrf24L01PCallback () {}

        /**
         * @brief onRx
         */
        virtual void onRx (uint8_t *data, size_t len) = 0;

        /**
         * IRQ when data was sent. If you do not need this method to be run in vain every time you send something, simply set MASK_TX_DS in
         * CONFIG register : nrfTx.setConfig (Nrf24L01P::MASK_TX_DS, x, y);
         */
        virtual void onTx () {}

        /**
         * IRQ when data was sent. If you do not need this method to be run in vain every time you send something, simply set MASK_MAX_RT in
         * CONFIG register : nrfTx.setConfig (Nrf24L01P::MASK_MAX_RT, x, y);
         */
        virtual void onMaxRt () {}
};

/**
 * States:
 * - Power Down (default after turning on).
 * - Standby 1 (CONFIG:PWR_UP bit set)
 * - RX Mode (it stays in it when CONFIG:PRIM_RX is set)
 * - TX Mode
 *
 * RX : constantly listens to valid packets (i.e. correct structure and correct address), and puts the data into FIFO.
 * If fifo is full, it discards new packets.
 *
 * TX :
 *
 * Various info:
 * - This "Enhanced ShockBurst" thing is ON by default (?)
 * - Static payload length is the default.
 * - An ACK packet can contain an optional payload from PRX to PTX. In order to use this feature, the
 * Dynamic Payload Length (DPL) feature must be enabled. W_ACK_PAYLOAD command is used for this.
 * - Shock Burst and Enchanced Shock Burst are two separate things. As far as I understand always either
 * of them is working. Differences are in features they provide and packet its format (most notably ESB
 * allows for bidirectional communication).
 *
 * Examples:
 *
 *   // Simplest : no auto-ack, static payloads
 *   Nrf24L01P nrfTx (&spiTx, &ceTx, &irqTx);
 *   nrfTx.setPayloadLength (0, PACKET_SIZE);
 *   nrfTx.setEnableDataPipe (Nrf24L01P::ERX_P0);
 *   nrfTx.powerUp (Nrf24L01P::TX);
 *
 *   // Still no auto-ack, but dynamic payloads
 *   Nrf24L01P nrfTx (&spiTx, &ceTx, &irqTx);
 *   nrfTx.setEnableDataPipe (Nrf24L01P::ERX_P0);
 *   nrfTx.setAutoRetransmit (Nrf24L01P::WAIT_1000_US, Nrf24L01P::RETRANSMIT_15);
 *   nrfTx.setEnableDynamicPayload (Nrf24L01P::DPL_P0);
 *   nrfTx.setFeature (Nrf24L01P::EN_DPL);
 *   nrfTx.powerUp (Nrf24L01P::TX);
 *
 *   // Dynamic payloads, and autoack (ack without payload)
 *   Nrf24L01P nrfTx (&spiTx, &ceTx, &irqTx);
 *   nrfTx.setAutoAck (Nrf24L01P::ENAA_P0);
 *   nrfTx.setEnableDataPipe (Nrf24L01P::ERX_P0);
 *   nrfTx.setAutoRetransmit (Nrf24L01P::WAIT_1000_US, Nrf24L01P::RETRANSMIT_15);
 *   nrfTx.setPayloadLength (0, PACKET_SIZE);
 *   nrfTx.powerUp (Nrf24L01P::TX);
 *
 *   // Dynamic pauloads and auto ack with dynamic payload (dynamic payload obligatory for auto-ack with payloads)
 *   Nrf24L01P nrfTx (&spiTx, &ceTx, &irqTx);
 *   nrfTx.setAutoAck (Nrf24L01P::ENAA_P0);
 *   nrfTx.setEnableDataPipe (Nrf24L01P::ERX_P0);
 *   nrfTx.setAutoRetransmit (Nrf24L01P::WAIT_1000_US, Nrf24L01P::RETRANSMIT_15);
 *   nrfTx.setEnableDynamicPayload (Nrf24L01P::DPL_P0);
 *   nrfTx.setFeature (Nrf24L01P::EN_ACK_PAY | Nrf24L01P::EN_DPL);
 *   nrfTx.powerUp (Nrf24L01P::TX);
 *   // ...
 *   nrfRx.setAckPayload (0, ackPayload, 3);
 *
 * Bugs:
 * - Sometimes I have to reset 2 boards at once. Othewise no communication is established.
 */
class Nrf24L01P {
public:
        enum { CHANNELS_NO_FOR_SANNER = 128 };

        enum Register {
                CONFIG = 0x00,
                EN_AA = 0x01,
                EN_RXADDR = 0x02,
                SETUP_AW = 0x03,
                SETUP_RETR = 0x04,
                RF_CH = 0x05,
                RF_SETUP = 0x06,
                STATUS = 0x07,
                OBSERVE_TX = 0x08,
                RPD = 0x09,
                RX_ADDR_P0 = 0x0A,
                RX_ADDR_P1 = 0x0B,
                RX_ADDR_P2 = 0x0C,
                RX_ADDR_P3 = 0x0D,
                RX_ADDR_P4 = 0x0E,
                RX_ADDR_P5 = 0x0F,
                TX_ADDR = 0x10,
                RX_PW_P0 = 0x11,
                RX_PW_P1 = 0x12,
                RX_PW_P2 = 0x13,
                RX_PW_P3 = 0x14,
                RX_PW_P4 = 0x15,
                RX_PW_P5 = 0x16,
                FIFO_STATUS = 0x17,
                DYNPD = 0x1C,
                FEATURE = 0x1D
        };

        enum ConfigBit { PRIM_RX = 0, PWR_UP = 1, CRCO = 2, EN_CRC = 3 /*, MASK_MAX_RT, MASK_TX_DS, MASK_RX_DR*/ };
        enum Status { TX_FULL_MASK = 0x01, RX_P_NO = 0x0e, MAX_RT = 1 << 4, TX_DS = 1 << 5, RX_DR = 1 << 6 };

        enum Nrf24L01PCommands {
                R_REGISTER = 0x00, /// Read command and status registers. Last 5 bits (AAAAA) are Register Map Address
                W_REGISTER = 0x20, /// Write command and status registers. Last 5 bits are Register Map Address. Execute in power down or standby
                                   /// modes only.
                REGISTER_MASK = 0x1F, /// Mask for the register address for commands listed above.
                R_RX_PAYLOAD = 0x61,  /// Read RX-payload: 1 – 32 bytes. A read operation always starts at byte 0. Payload is deleted from FIFO
                                      /// after it is read. Used in RX mode. See R_RX_PL_WID.
                W_TX_PAYLOAD = 0xA0,  /// Write TX-payload: 1 – 32 bytes. A write operation always starts at byte 0 used in TX payload.
                FLUSH_TX = 0xE1,
                FLUSH_RX = 0xE2,
                REUSE_TX_PL = 0xE3,
                R_RX_PL_WID = 0x60,   /// Read RX payload width for the top R_RX_PAYLOAD in the RX FIFO. Note: Flush RX FIFO if the read value is
                                      /// larger than 32 bytes. The bits in the FEATURE register shown in Table 28. on page 63 have to be set for
                                      /// this command to work.
                W_ACK_PAYLOAD = 0xa8, /// Used in RX mode. Write Payload to be transmitted together with ACK packet on PIPE PPP. (PPP valid in
                                      /// the range from 000 to 101). Maximum three ACK packet payloads can be pending. Payloads with same PPP
                                      /// are handled using first in -first out principle. Write payload: 1– 32 bytes. A write operation always
                                      /// starts at byte 0. The bits in the FEATURE register shown in Table 28. on page 63 have to be set.
                W_TX_PAYLOAD_NO_ACK = 0xb0, /// Used in TX mode. Disables AUTOACK on this specific packet (if auto ack is ON). The bits in the
                                            /// FEATURE register shown in Table 28. on page 63 have to be set.
                NOP = 0xFF
        };

        enum MaskIrqSource {
                MASK_RX_DR = 1 << 6, // Block RX IRQ
                MASK_TX_DS = 1 << 5, // Block TX IRQ
                MASK_MAX_RT = 1 << 4,
                MASK_ALL_IRQ = MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT,
                MASK_NO_IRQ = 0
        };

        Nrf24L01P (Spi *spi, Gpio *cePin, Gpio *irqPin, uint32_t bogoDelay = 0);

        enum CrcLength { CRC_LEN_1 = 0, CRC_LEN_2 = 1 << CRCO };
        void setConfig (uint8_t maskIrqSource, bool crcEnable, CrcLength crcLength);

        enum Mode { TX, RX };
        void powerUp (Mode mode);

        enum EnAa { ENAA_P5 = 1 << 5, ENAA_P4 = 1 << 4, ENAA_P3 = 1 << 3, ENAA_P2 = 1 << 2, ENAA_P1 = 1 << 1, ENAA_P0 = 1 << 0 };
        void setAutoAck (uint8_t enAa) { writeRegister (EN_AA, enAa); }

        enum EnRxAddr { ERX_P5 = 1 << 5, ERX_P4 = 1 << 4, ERX_P3 = 1 << 3, ERX_P2 = 1 << 2, ERX_P1 = 1 << 1, ERX_P0 = 1 << 0 };
        void setEnableDataPipe (uint8_t pipes) { writeRegister (EN_RXADDR, pipes); }

        enum AddressWidth { WIDTH_3 = 0x01, WIDH_4 = 0x02, WIDTH_5 = 0x03 };
        void setAdressWidth (AddressWidth aw) { writeRegister (SETUP_AW, aw); }

        enum RetransmitDelay {
                WAIT_250_US,
                WAIT_500_US,
                WAIT_750_US,
                WAIT_1000_US,
                WAIT_1250_US,
                WAIT_1500_US,
                WAIT_1750_US,
                WAIT_2000_US,
                WAIT_2250_US,
                WAIT_2500_US,
                WAIT_2750_US,
                WAIT_3000_US,
                WAIT_3250_US,
                WAIT_3500_US,
                WAIT_3750_US,
                WAIT_4000_US
        };

        enum RetransmitCount {
                RETRANSMIT_0,
                RETRANSMIT_1,
                RETRANSMIT_2,
                RETRANSMIT_3,
                RETRANSMIT_4,
                RETRANSMIT_5,
                RETRANSMIT_6,
                RETRANSMIT_7,
                RETRANSMIT_8,
                RETRANSMIT_9,
                RETRANSMIT_10,
                RETRANSMIT_11,
                RETRANSMIT_12,
                RETRANSMIT_13,
                RETRANSMIT_14,
                RETRANSMIT_15
        };

        /**
         * Works only in Enchanced Shock Burst.
         * Please take care when setting this parameter. If the ACK payload is more than 15 byte in 2Mbps mode the ARD must be 500μS or more, if
         * the ACK payload is more than 5byte in 1Mbps mode the ARD must be 500μS or more. In 250kbps mode (even when the payload is not in ACK)
         * the ARD must be 500μS or more. Please see section 7.4.2 on page 33 for more information.
         */
        void setAutoRetransmit (RetransmitDelay d, RetransmitCount c) { writeRegister (SETUP_RETR, (d << 4) | c); }

        void setChannel (uint8_t channel) { writeRegister (RF_CH, channel); }

        enum DataRate { MBPS_1 = 0, MBPS_2 = 1 << 3, KBPS_250 = 1 << 5 };
        enum Gain { DBM_18 = 0 << 1, DBM_12 = 1 << 1, DBM_6 = 2 << 1, DBM_0 = 3 << 1 };
        void setDataRate (DataRate dr, Gain g) { writeRegister (RF_SETUP, dr | g); }

        enum IrqSource { /*RX_DR = 1 << 6, TX_DS = 1 << 5, MAX_RT = 1 << 4,*/ IRQ_ALL = RX_DR | TX_DS | MAX_RT };
        uint8_t getStatus () const { return nop (); }

        void getObserve (uint8_t *lostPackets, uint8_t *retransmittedPackets) const
        {
                *lostPackets = readRegister (OBSERVE_TX);
                *retransmittedPackets = *lostPackets & 0x0f;
                *lostPackets >>= 4;
        }

        bool getReceivedPowerDetector () const { return readRegister (RPD) & 1; }
        void setRxAddress (uint8_t dataPipeNo, uint8_t const *address, uint8_t addressLen)
        {
                writeRegister (RX_ADDR_P0 + dataPipeNo, address, addressLen);
        }

        void setTxAddress (uint8_t const *address, uint8_t addressLen) { writeRegister (TX_ADDR, address, addressLen); }

        void setPayloadLength (uint8_t dataPipeNo, uint8_t len) { writeRegister (RX_PW_P0 + dataPipeNo, len); }

        /// Fifo status values. Use with getFifoStatus.
        enum FifoStatus {
                TX_REUSE = 1 << 6, /// Used for a PTX device. Pulse the rfce high for at least 10μs to Reuse last transmitted payload. TX payload
                                   /// reuse is active until W_TX_PAYLOAD or FLUSH TX is executed. TX_REUSE is set by the SPI command REUSE_TX_PL
                                   /// , and is reset by the SPI commands W_TX_PAYLOAD or FLUSH TX.
                TX_FULL = 1 << 5,  /// TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO.
                TX_EMPTY = 1 << 4, /// TX FIFO empty flag. 1: TX FIFO empty. 0: Data in TX FIFO.
                RX_FULL = 1 << 1,  /// RX FIFO full flag. 1: RX FIFO full. 0: Available locations in RX FIFO.
                RX_EMPTY = 1 << 0  /// RX FIFO empty flag. 1: RX FIFO empty. 0: Data in RX FIFO.
        };

        /// Gets fifo satus. Mask with FifoStatus enum values to check for something useful.
        uint8_t getFifoStatus () const { return readRegister (FIFO_STATUS); }

        /// DynamicPayloadLength
        enum DynamicPayloadLength { DPL_P5 = 1 << 5, DPL_P4 = 1 << 4, DPL_P3 = 1 << 3, DPL_P2 = 1 << 2, DPL_P1 = 1 << 1, DPL_P0 = 1 << 0 };

        /// Sets DynamicPayloadLength. setFeature (EN_DPL) must be set for this to work.
        void setEnableDynamicPayload (uint8_t dpl) { writeRegister (DYNPD, dpl); }

        /// Feature register values, to be used with setFeature.
        enum Feature {
                EN_DPL = 1 << 2,     /// Enables Dynamic Payload Length
                EN_ACK_PAY = 1 << 1, /// Enables Payload with ACK
                EN_DYN_ACK = 1       /// Enables the W_TX_PAYLOAD_NOACK command
        };

        /// Feature
        void setFeature (uint8_t f) { writeRegister (FEATURE, f); }

        void setCallback (Nrf24L01PCallback *c) { callback = c; }
        void poorMansScanner (int tries);

        /*****************************************************************************/
        /* Commands.                                                                 */
        /*****************************************************************************/

        /**
         * @brief transmit
         * @param data The data.
         * @param len Length of the data obviously.
         * @param noAck If true, it disables auto-ack feature for this particular packet. Has effect only if auto-ack is active.
         */
        void transmit (uint8_t *data, size_t len, bool noAck = false);

        /**
         * Sets payload when using auto-ack with dynamic payload length in device in PRX mode.
         * This can set up to 3 payloads (since TX FIFO has 3 slots) that will be sent to the
         * PTX. setFeature (EN_DPL | EN_ACK_PAY) must be set for this to work.
         * @param forPipe Pipe number from 0 to 5.
         */
        void setAckPayload (uint8_t forPipe, uint8_t *data, size_t len);

        /**
         * @brief receive Gets the data somebody sent to us over the air.
         * @param data Input buffer.
         * @param len
         * @return Pointer to some place in the input buffer. Beware, that ret != data !!!
         */
        uint8_t *receive (uint8_t *data, size_t len);

        void flushTx ();
        void flushRx ();

        /**
         * @brief Read RX payload width for the top R_RX_PAYLOAD in the RX FIFO. According to the datasheet (as far as I understood) this feature
         * only works if dynamic payload length is turned on, but my experiments show, that it works for static payloads lengths also, which is
         * very convenient! Note: Flush RX FIFO if the read value is larger than 32 bytes.
         */
        size_t getPayloadLength () const;

        /**
         * Used for a PTX device. Reuse last transmitted payload. TX payload reuse is active until W_TX_PAYLOAD or FLUSH TX is executed. TX
         * payload reuse must not be activated or deacti- vated during package transmission.
         *
         * I've never used it personally but I think this is useful if you don't need to send anything, but you want to receive ACKs with
         * payloads. Using this method you can avoid transfering data to TX FIFO in vain.
         */
        void reuseTx ();

        /// No Operation. Might be used to read the STATUS register.
        uint8_t nop () const;

        /// Bogo-delay between bytes sent over SPI.
        void setBogoDelay (uint32_t b) { bogoDelay = b; }

//private:
        void writeRegister (uint8_t reg, uint8_t value);
        void writeRegister (uint8_t reg, uint8_t const *data, uint8_t len);
        uint8_t readRegister (uint8_t reg) const;

        /*****************************************************************************/

        void setCe (bool b) { cePin->set (b); }
        void doBogoDelay () const;

private:
        Spi *spi;
        Gpio *cePin;
        Gpio *irqPin;
        uint8_t configRegisterCopy;
        Nrf24L01PCallback *callback;
        uint8_t bufferRx[33];
        uint32_t bogoDelay;
};

#endif // NRF24L0P_H
