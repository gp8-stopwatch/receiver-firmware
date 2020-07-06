/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Config.h"

namespace cfg {

bool &changed ()
{
        static bool h = true; /// Says if the system should pay attention for settings that has been changed. Not user-settable.
        return h;
}

} // namespace cfg