/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Engine.h"

namespace lm {

void UsartChar::run ()
{
        if (usart.sink) {
                usart.sink->onData (static_cast<uint8_t> (c));
        }
        else if (usart.onData) {
                usart.onData (static_cast<uint8_t> (c));
        }
}

//void UsartString::run ()
//{
//        if (usart.sink) {
//                for (char c : string) {
//                        usart.sink->onData (static_cast<uint8_t> (c));
//                }
//        }
//        else if (usart.onData) {
//                for (char c : string) {
//                        usart.onData (static_cast<uint8_t> (c));
//                }
//        }
//}

void UsartString<const char *>::run ()
{
        if (usart.sink) {
                for (char c : string) {
                        usart.sink->onData (static_cast<uint8_t> (c));
                }
        }
        else if (usart.onData) {
                for (char c : string) {
                        usart.onData (static_cast<uint8_t> (c));
                }
        }
}

} // namespace lm
