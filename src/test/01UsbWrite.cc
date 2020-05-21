/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include <cstdint>
#include <cstring>

#define CDC_DATA_OUT_MAX_PACKET_SIZE USB_FS_MAX_PACKET_SIZE /* don't exceed USB_FS_MAX_PACKET_SIZE; Linux data loss happens otherwise */
#define CDC_DATA_IN_MAX_PACKET_SIZE 256
#define CDC_CMD_PACKET_SIZE 8 /* this may need to be enlarged for advanced CDC commands */
#define INBOUND_BUFFER_SIZE (4 * CDC_DATA_IN_MAX_PACKET_SIZE)

struct Context {
        uint32_t begin;
        uint32_t end;
        uint32_t InboundBuffer[(INBOUND_BUFFER_SIZE) / sizeof (uint32_t)];
};

static Context context[1];

void usbWriteData (const uint8_t *str, size_t size)
{
        // if (!context->ready) {
        //         return;
        // }

        // __disable_irq ();

        int begin = context->begin;
        int end = context->end;
        int actualSize = 0;

        if (begin == end) {
                actualSize = 0;
        }
        else if (end > begin) {
                actualSize = end - begin;
        }
        else {
                actualSize = INBOUND_BUFFER_SIZE - begin + end - 1;
        }

        // We cant let the begin and end to be equal when filing the buffer, because this means the buffer is empty
        if (actualSize + size >= INBOUND_BUFFER_SIZE) {
                // __enable_irq ();
                return; // flase
        }

        if (end + size > INBOUND_BUFFER_SIZE) {
                int tail = INBOUND_BUFFER_SIZE - end;
                memcpy (((uint8_t *)context[0].InboundBuffer) + end, str, tail);

                int head = size - tail;
                memcpy (((uint8_t *)context[0].InboundBuffer), str + tail, head);
                context->end = head;
        }
        else {
                memcpy (((uint8_t *)context[0].InboundBuffer) + end, str, size);
                context->end += size;
        }

        // __enable_irq ();
}

/****************************************************************************/

void usbWrite (const char *str) { usbWriteData ((const uint8_t *)str, strlen (str)); }

/****************************************************************************/

uint8_t USBD_CDC_SOF ()
{
        uint32_t buffsize = 0;
        // USBD_CDC_HandleTypeDef *hcdc = context;
        auto *hcdc = &context[0];

        if (hcdc->begin != hcdc->end) {

                if (hcdc->begin > hcdc->end) {
                        buffsize = INBOUND_BUFFER_SIZE - hcdc->begin;
                }
                else {
                        /* send all data between read index and write index */
                        buffsize = hcdc->end - hcdc->begin;
                }

                if (true /* USBD_CDC_TransmitPacket (pdev, 0, hcdc->begin, buffsize) == USBD_OK */) {
                        hcdc->begin += buffsize;

                        /* if we've reached the end of the buffer, loop around to the beginning */
                        if (hcdc->begin >= INBOUND_BUFFER_SIZE) {
                                hcdc->begin = 0;
                        }

                        if (hcdc->end >= INBOUND_BUFFER_SIZE) {
                                hcdc->end = 0;
                        }
                }
        }

        /* if there is a lingering request needed due to a HAL_BUSY, retry it */
        // if (hcdc->OutboundTransferNeedsRenewal) {
        //         USBD_CDC_ReceivePacket (pdev, 0);
        // }

        // return USBD_OK;
        return 1;
}

/****************************************************************************/

/**
 * Storage of the data in a circullar buffer.
 */
TEST_CASE ("StorageHelloWorld", "[usb]")
{
        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 0);

        usbWrite ("Ala ma kota, a kot ma ale\r\n"); // 27

        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 27);

        usbWrite ("Ala ma kota, a kot ma ale\r\n");

        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 2 * 27);

        for (int i = 0; i < 35; ++i) {
                usbWrite ("Ala ma kota, a kot ma ale\r\n");
        }

        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 37 * 27);

        // This won't fit.
        usbWrite ("Ala ma kota, a kot ma ale\r\n");

        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 37 * 27);
}

/**
 *
 */
TEST_CASE ("StorageAndRetrieval", "[usb]")
{
        context->begin = 0;
        context->end = 0;
        memset (context->InboundBuffer, 0, INBOUND_BUFFER_SIZE);

        usbWrite ("Ala ma kota, a kot ma ale\r\n");
        // Get everything from the buffer and pretend sending.
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 27);
        REQUIRE (context->end == 27);

        usbWrite ("Ala ma kota, a kot ma ale\r\n");
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 2 * 27);
        REQUIRE (context->end == 2 * 27);

        for (int i = 0; i < 35; ++i) {
                usbWrite ("Ala ma kota, a kot ma ale\r\n");
        }
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 37 * 27);
        REQUIRE (context->end == 37 * 27);

        // Data will wrap arond now. Lets check if correctly.
        usbWrite ("Ala ma kota, a kot ma ale\r\n");

        REQUIRE (context->begin == 37 * 27); // Same as last time (999)
        REQUIRE (context->end == 2);

        USBD_CDC_SOF ();
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 2);
        REQUIRE (context->end == 2);

        for (int i = 0; i < 16; ++i) {
                usbWrite ("Ala ma kota, a kot ma ale\r\n");
        }
        USBD_CDC_SOF ();
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 2 + 16 * 27);
        REQUIRE (context->end == 2 + 16 * 27);

        // Again insert full buffer.
        for (int i = 0; i < 37; ++i) {
                usbWrite ("Ala ma kota, a kot ma ale\r\n");
        }
        USBD_CDC_SOF ();
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 409);
        REQUIRE (context->end == 409);
}

TEST_CASE ("CornerCase", "[usb]")
{
        context->begin = 0;
        context->end = 0;
        memset (context->InboundBuffer, 0, INBOUND_BUFFER_SIZE);

        for (int i = 0; i < 1023; ++i) {
                usbWrite ("a");
        }
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 1023);
        REQUIRE (context->end == 1023);

        for (int i = 0; i < 1023; ++i) {
                usbWrite ("a");
        }
        USBD_CDC_SOF ();
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 1022);
        REQUIRE (context->end == 1022);
}

TEST_CASE ("CornerCase2", "[usb]")
{
        context->begin = 0;
        context->end = 0;
        memset (context->InboundBuffer, 0, INBOUND_BUFFER_SIZE);

        for (int i = 0; i < 1000; ++i) {
                usbWrite ("a");
        }
        USBD_CDC_SOF ();

        REQUIRE (context->begin == 1000);
        REQUIRE (context->end == 1000);

        usbWrite ("123456789012345678901234");
        REQUIRE (context->begin == 1000);
        REQUIRE (context->end == 1024);

        USBD_CDC_SOF ();

        REQUIRE (context->begin == 0);
        REQUIRE (context->end == 0);
}