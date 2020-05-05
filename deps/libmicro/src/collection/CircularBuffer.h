/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SRC_IO_BUFFER_H_
#define SRC_IO_BUFFER_H_

#include <cstddef>
#include <cstdint>

/**
 * Circular queue designed for storing one uniform chunk of bytes (thus name buffer). Oldest bytes
 * can be read and removed, and new bytes can be added at the end.
 * Bufor kolejka. Gdy miejsce się kończy na końcu bufora, zaczyna zapisywać znów na początku.
 * Elementy są jednorodne, to znaczy sama kolejka nie definiuje żadnego logicznego (abstrakcyjnego)
 * elementu w środku. To jest po prostu ciąg bajtów, czy znaków, czy elementów.
 * Zapisywane do niego bajty lądują zawsze na końcu, a odczytywane bajty pobierane
 * są z początku. Jeżeli następuje przepełnienie bufora, to bufor może zwrócić false i nie dodać
 * więcej danych, albo może nadpisać najstarzse dane.
 */
class CircularBuffer {
public:
        /**
         * @brief Alokuje pamięć dla tego bufora.
         * @param maxSize Rozmiar bufora. W pamięci zostanie zaalokowane *dokładnie* tyle bajtów w tablicy.
         *
         * @param canOverwrite Domyślnie jest false. Jeśli jest true, to można zapisać do bufora nieograniczoną
         * liczbę bajtów, ale najstarsze bajty się zamazują. Aby dowiedzieć się ile się zamazało, należy zapisać
         * sobie liczbę wolnych bajtów (free = maxSize () - size ()), dodać nowe bajty i odjąć free.
         */
        CircularBuffer (size_t maxSizeB, bool canOverwrite = false);
        ~CircularBuffer ();
        size_t store (uint8_t const *data, size_t len, bool nullTerminate = false);
        bool removeLast (size_t len);
        void retrieve (uint8_t **partA, size_t *lenA, uint8_t **partB, size_t *lenB, size_t maxBytes = -1) const;
        size_t declareRead(size_t len);
        void declareReadAll ();
        void clear ()
        {
                offsetIn = offsetOut = 0;
                empty = true;
        }

        /// W bajtach
        size_t size () const;

        void declareWrite (size_t s)
        {
                empty = false;
                offsetOut = 0;
                offsetIn = s;
        }

        /// W bajtach
        size_t maxSize () const { return MAX_SIZE; }
        uint8_t *data () { return buffer; }

#if !defined(UNIT_TEST)
private:
#endif

        const size_t MAX_SIZE;
        size_t offsetIn;
        size_t offsetOut;
        bool empty;
        bool canOverwrite;
        uint8_t *buffer;
};

// extern uint32_t ioBuferSum (IoBuffer *b);

#endif
