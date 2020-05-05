/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CircularBuffer.h"
#include <cstring>
//#include "errorHandler.h"
//#include "io_buffer.h"

CircularBuffer::CircularBuffer (size_t maxSizeB, bool canOverwrite)
    : MAX_SIZE (maxSizeB), offsetIn (0), offsetOut (0), empty (true), canOverwrite (canOverwrite)
{
        // TODO zastanowić się, bo tu było new uint8_t. Zastanowić się z tym alignmentem do 4.
        buffer = (uint8_t *)new uint32_t[MAX_SIZE / 4];
}

/*****************************************************************************/

CircularBuffer::~CircularBuffer ()
{
        // W tej implementacji nigdy się nie skasują.
        delete[](uint32_t *) buffer;
}

/*****************************************************************************/

size_t CircularBuffer::store (uint8_t const *data, size_t len, bool nullTerminate)
{
        if (!len) {
                return 0;
        }

        size_t overwrite = 0;
        if (size () + len > MAX_SIZE) {
                if (!canOverwrite) {
                        // return 0;
                        len = MAX_SIZE - size ();
                }
                else {
                        overwrite = (size () + len) - MAX_SIZE;
                }
        }

        empty = false;
        // Kopiuje Z buf do txBuffer zaczynając od offsetIn. Uwaga, offsetIn i offsetOut obydwa odnoszą się do transmisji i do txBuffer.
        if (offsetIn + len <= MAX_SIZE) {
                memcpy (buffer + offsetIn, data, len);
                offsetIn += len;

                if (nullTerminate) {
                        *(buffer + offsetIn - 1) = '\0';
                }
        }
        else {
                // Kopiuj od offsetIn do końca bufora a następnie...
                size_t tempLen = MAX_SIZE - offsetIn;
                memcpy (buffer + offsetIn, data, tempLen);

                // Zawiń i kopiuj od początku.
                memcpy (buffer, data + tempLen, len - tempLen);
                offsetIn = len - tempLen;

                if (nullTerminate) {
                        if (offsetIn) {
                                *(buffer + offsetIn - 1) = '\0';
                        }
                        else {
                                *(buffer + MAX_SIZE - 1) = '\0';
                        }
                }
        }

        offsetIn %= MAX_SIZE;

        if (overwrite) {
                offsetOut = offsetIn;
        }

        return len;
}

/*****************************************************************************/

size_t CircularBuffer::declareRead (size_t len)
{
        if (len > size ()) {
                len = size ();
        }

        offsetOut = (offsetOut + len) % MAX_SIZE;
        empty = (offsetIn == offsetOut);

        return len;
}

/*****************************************************************************/

void CircularBuffer::declareReadAll ()
{
        offsetOut = offsetIn;
        empty = true;
}

/*****************************************************************************/

bool CircularBuffer::removeLast (size_t len)
{
        if (len > size () || !len) {
                return false;
        }

        if (offsetIn >= len) {
                offsetIn = (offsetIn - len);
        }
        else {
                offsetIn = MAX_SIZE - len + offsetIn;
        }

        empty = (offsetIn == offsetOut);
        return true;
}

/*****************************************************************************/

void CircularBuffer::retrieve (uint8_t **partA, size_t *lenA, uint8_t **partB, size_t *lenB, size_t maxBytes) const
{
        // Gdy offsetOut == offsetIn, to znaczy że jest pusty.
        if (offsetOut < offsetIn) {
                size_t tmp = offsetIn - offsetOut;                   // Proste odejmowanie, bo offsetIn > offsetOut.
                *lenA = (maxBytes < tmp) ? (maxBytes) : (tmp);       // Ale jeśli user podał ograniczenie maxBytes, to obetnij do maxBytes.
                *partA = (*lenA) ? (buffer + offsetOut) : (nullptr); // Jeżeli maxBytes == 0, to lenA == 0 i wówczas zwracamy pusty wskaźnik.
                *partB = nullptr;                                    // Zasze pusty.
                *lenB = 0;
        }
        else if (offsetOut > offsetIn || (offsetOut == offsetIn && !empty)) {
                size_t tmp = MAX_SIZE - offsetOut; // Rozmiar pierwzego kawałka, czyli zajętej końcówki bufora (bo się zawinęło).
                tmp = (maxBytes < tmp) ? (maxBytes) : (tmp);       // Ograniczamy ten rozmiar do maxBytes.
                *lenA = tmp;                                       // Usatwaiamy go do wskaźnika, czyli de facto zwracmy.
                maxBytes -= tmp;                                   // Zwróciliśmy już tmp bajtów, czyli pomniejszamy maxBytes o tmp.
                *partA = (tmp) ? (buffer + offsetOut) : (nullptr); // Zwracamy pusty wskaźnik do bufora partA, jeśli maxBytes == 0.

                *lenB = (maxBytes < offsetIn) ? (maxBytes) : (offsetIn); // Analogicznie dla partB.
                *partB = (offsetIn) ? (buffer) : (nullptr);
                *partB = (*lenB) ? (*partB) : nullptr;
        }
        else { // PUSTY!
                *partA = nullptr;
                *lenA = 0;
                *partB = nullptr;
                *lenB = 0;
        }
}

/*****************************************************************************/

size_t CircularBuffer::size () const
{
        if (empty) {
                return 0;
        }

        if (offsetIn > offsetOut) {
                return offsetIn - offsetOut;
        }

        return MAX_SIZE - offsetOut + offsetIn;
}

/*****************************************************************************/

// bool io_buffer_store (io_buffer_t buffer, uint8_t *data, size_t len)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        return b->store (data, len);
//}

///*****************************************************************************/

// void io_buffer_retrieve (io_buffer_t buffer, uint8_t **partA, size_t *lenA, uint8_t **partB, size_t *lenB, size_t maxBytes)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        b->retrieve (partA, lenA, partB, lenB, maxBytes);
//}

///*****************************************************************************/

// void io_buffer_declare_read (io_buffer_t buffer, size_t len)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        b->declareRead (len);
//}

///*****************************************************************************/

// void io_buffer_declare_read_all (io_buffer_t buffer)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        b->declareReadAll ();
//}

///*****************************************************************************/

// size_t io_buffer_size (io_buffer_t buffer)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        return b->size ();
//}

///*****************************************************************************/

// size_t io_buffer_max_size (io_buffer_t buffer)
//{
//        IoBuffer *b = static_cast<IoBuffer *> (buffer);
//        return b->maxSize ();
//}

///*****************************************************************************/

// uint32_t ioBuferSum (IoBuffer *b)
//{
//        uint32_t ret = 0;
//        size_t size = b->size ();
//        uint8_t *p = b->data ();

//        for (size_t i = 0; i < size; ++i) {
//                ret += *(p + i);
//        }

//        return ret;
//}
