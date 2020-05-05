/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_CONSOLE_H
#define LIB_MICRO_CONSOLE_H

#include "Usart.h"
#include "character/ICharacterInput.h"
#include "character/ICharacterOutput.h"
#include "character/ICharacterSink.h"
#include "collection/CharacterCircularQueue.h"
#include <cstdint>

/**
 * A basic input console, LF end-of-lines.
 */
class Console : public ICharacterSink {
public:
        enum { MAX_RX_BUFFER = 128 };

        Console (bool echo = true, const char *prompt = "$");
        virtual ~Console () { delete[] rxBuffer; }

        /**
         * @brief To be called in the main loop.
         */
        void run ();

        void setInput (ICharacterInput *i);
        void setOutput (ICharacterOutput *o);

protected:
        virtual void onNewLine (const char *str, size_t len);

private:
        /**
         * @brief Can be called by USART ISR or something similar.
         * @param c
         */
        void onData (char c);

protected:
        ICharacterInput *input;
        ICharacterOutput *output;

private:
        int rxBufferPos;
        char *rxBuffer;
        // Collection of lines
        CharacterCircularQueue<256> lineBuffer;
        const char *prompt;
        bool echo;
};

#endif // CONSOLE_H
