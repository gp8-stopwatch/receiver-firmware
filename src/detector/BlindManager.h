/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Timer.h"

/**
 * Blind time management in separate class to minimize coupling.
 */
class BlindManager {
public:
        bool isBlind () const { return !timer.isExpired (); }
        bool isSeeing () const { return timer.isExpired (); }

        void start () { timer.start (getConfig ().getBlindTime ()); }

private:
        mutable Timer timer;
};