/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Nrf24L01P.h"
#include "Debug.h"
#include <cstring>

/*****************************************************************************/

Nrf24L01P::Nrf24L01P (Spi *spi, Gpio *cePin, Gpio *irqPin, uint32_t bogoDelay)
    : spi (spi), cePin (cePin), irqPin (irqPin), configRegisterCopy (0x08), callback (nullptr), bogoDelay (bogoDelay)
{
        if (irqPin) {
                irqPin->setOnToggle ([this] {
                        uint8_t s = getStatus ();

                        /*
                         * From the datasheet:
                         * The RX_DR IRQ is asserted by a new packet arrival event. The procedure for handling this interrupt should
                         * be: 1) read payload through SPI, 2) clear RX_DR IRQ, 3) read FIFO_STATUS to check if there are more
                         * payloads available in RX FIFO, 4) if there are more data in RX FIFO, repeat from step 1).
                         *
                         * Note: Always check if the packet width reported is 32 bytes or shorter when using the R_RX_PL_WID command. If its
                         * width is longer than 32 bytes then the packet contains errors and must be discarded. Discard the packet by using the
                         * Flush_RX command.
                         */
                        if (s & RX_DR) {
                                do {
                                        size_t payloadLen = getPayloadLength ();

                                        if (payloadLen > 32 || payloadLen <= 0) {
                                                flushRx ();
                                                writeRegister (Nrf24L01P::STATUS, RX_DR);
                                                return;
                                        }

                                        uint8_t *out = receive (bufferRx, payloadLen);

                                        // Clear status
                                        writeRegister (Nrf24L01P::STATUS, RX_DR);

                                        if (callback) {
                                                callback->onRx (out, payloadLen);
                                        }
                                } while (!(readRegister (Nrf24L01P::FIFO_STATUS) & RX_EMPTY));
                        }

                        /*
                         * Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX. If AUTO_ACK is activated, this bit is set high
                         * only when ACK is received. Write 1 to clear bit.
                         *
                         * You can use : nrfTx.setConfig (Nrf24L01P::MASK_TX_DS, x, y) if callback->onTx () is not necessary.
                         */
                        if (s & TX_DS) {
                                if (callback) {
                                        callback->onTx ();
                                }

                                // Clear status
                                writeRegister (Nrf24L01P::STATUS, TX_DS);
                        }

                        /*
                         * Maximum number of TX retransmits interrupt Write 1 to clear bit. If MAX_RT is asserted it must be cleared to enable
                         * further communication.
                         */
                        if (s & MAX_RT) {
                                if (callback) {
                                        callback->onMaxRt ();
                                }

                                // Clear status
                                writeRegister (Nrf24L01P::STATUS, MAX_RT);
                        }
                });
        }
}

/*****************************************************************************/

void Nrf24L01P::setConfig (uint8_t maskIrqSource, bool crcEnable, CrcLength crcLength)
{
        configRegisterCopy &= (1 << PWR_UP | 1 << PRIM_RX);
        configRegisterCopy |= maskIrqSource | (uint8_t (crcEnable) << EN_CRC) | crcLength;
        writeRegister (CONFIG, configRegisterCopy);
}

/*****************************************************************************/

void Nrf24L01P::writeRegister (uint8_t reg, uint8_t value)
{
        spi->setNss (false);
        spi->transmit8 (reg | W_REGISTER);
        doBogoDelay ();
        spi->transmit8 (value);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

void Nrf24L01P::writeRegister (uint8_t reg, uint8_t const *data, uint8_t len)
{
        spi->setNss (false);
        spi->transmit8 (reg | W_REGISTER);
        doBogoDelay ();
        spi->transmit8 (data, len, nullptr, bogoDelay, false);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

uint8_t Nrf24L01P::readRegister (uint8_t reg) const
{
        spi->setNss (false);
        spi->transmit8 (reg | R_REGISTER);
        doBogoDelay ();
        uint8_t ret = spi->transmit8 (NOP);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
        return ret;

        //        spi->setNss (false);
        //        uint8_t in, out;
        //        out = reg | R_REGISTER;
        //        HAL_SPI_TransmitReceive (&spi->spiHandle, &out, &in, 1, 1000);
        //        doBogoDelay ();
        //        out = NOP;
        //        HAL_SPI_TransmitReceive (&spi->spiHandle, &out, &in, 1, 1000);
        //        doBogoDelay ();
        //        spi->setNss (true);
        //        doBogoDelay ();
        //        return out;
}

/*****************************************************************************/

void Nrf24L01P::powerUp (Mode mode)
{
        configRegisterCopy |= (1 << PWR_UP) | mode;
        writeRegister (CONFIG, configRegisterCopy);

        // Start listenig right away
        if (mode == RX) {
                setCe (true);
        }

        // Clear FIFOS
        flushTx ();
        doBogoDelay ();
        flushRx ();
        doBogoDelay ();

        // Clear IRQS
        writeRegister (STATUS, 0x70);
}

/*****************************************************************************/

void Nrf24L01P::flushTx ()
{
        spi->setNss (false);
        spi->transmit8 (FLUSH_TX);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

void Nrf24L01P::flushRx ()
{
        spi->setNss (false);
        spi->transmit8 (FLUSH_RX);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

void Nrf24L01P::reuseTx ()
{
        spi->setNss (false);
        spi->transmit8 (REUSE_TX_PL);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

uint8_t Nrf24L01P::nop () const
{
        spi->setNss (false);
        uint8_t r = spi->transmit8 (NOP);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
        return r;
}

/*****************************************************************************/

void Nrf24L01P::transmit (uint8_t *data, size_t len, bool noAck)
{
        spi->setNss (false);

        if (noAck) {
                spi->transmit8 (W_TX_PAYLOAD_NO_ACK);
        }
        else {
                spi->transmit8 (W_TX_PAYLOAD);
        }

        doBogoDelay ();
        spi->transmit8 (data, len, nullptr, bogoDelay, false);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();

        setCe (true);
        doBogoDelay ();
        setCe (false);
}

/*****************************************************************************/

void Nrf24L01P::setAckPayload (uint8_t forPipe, uint8_t *data, size_t len)
{

        spi->setNss (false);
        spi->transmit8 (W_ACK_PAYLOAD | forPipe);
        doBogoDelay ();
        spi->transmit8 (data, len, nullptr, bogoDelay, false);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
}

/*****************************************************************************/

size_t Nrf24L01P::getPayloadLength () const
{
        spi->setNss (false);
        spi->transmit8 (R_RX_PL_WID);
        doBogoDelay ();
        uint8_t r = spi->transmit8 (NOP);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
        return r;
}

/*****************************************************************************/

uint8_t *Nrf24L01P::receive (uint8_t *data, size_t len)
{
        spi->setNss (false);
        spi->transmit8 (R_RX_PAYLOAD);
        doBogoDelay ();
        spi->receive8 (data, len, bogoDelay, false);
        doBogoDelay ();
        spi->setNss (true);
        doBogoDelay ();
        return data;
}

/*****************************************************************************/

void Nrf24L01P::poorMansScanner (int tries)
{
        uint8_t channel[CHANNELS_NO_FOR_SANNER];
        memset (channel, 0, CHANNELS_NO_FOR_SANNER);
        Debug *d = Debug::singleton ();

        for (int j = 0; j < tries; ++j) {
                for (int i = 0; i < CHANNELS_NO_FOR_SANNER; ++i) {
                        // select a new channel
                        // setChannel ((128 * i) / CHANNELS_NO_FOR_SANNER);
                        setChannel (i);

                        // switch on RX
                        powerUp (RX);

                        // wait enough for RX-things to settle
                        HAL_Delay (1);

                        // this is actually the point where the RPD-flag
                        // is set, when CE goes low
                        setCe (false);

                        // read out RPD flag; set to 1 if
                        // received power > -64dBm
                        uint8_t rpd = readRegister (RPD);

                        if (rpd > 0) {
                                ++channel[i];
                        }

                        //                        HAL_Delay (1);
                }
        }

        for (int i = 0; i < CHANNELS_NO_FOR_SANNER; ++i) {
                d->print (i);
                d->print (" : ");

                for (int j = 0; j < channel[i]; ++j) {
                        d->print ("#");
                }

                d->print ("\n");
        }
}

/*****************************************************************************/

void Nrf24L01P::doBogoDelay () const
{
        for (size_t i = 0; i < bogoDelay; ++i) {
                __asm("nop");
        }
}
