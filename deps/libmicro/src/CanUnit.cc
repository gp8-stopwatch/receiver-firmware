/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "CanUnit.h"
#include <thread>
#include <unistd.h>

/*****************************************************************************/

Can::Can (std::vector<Entry> const &entries, ICanCallback *c) : entries (entries), cnt (0), callback (c)
{
        if (!entries.empty ()) {
                timer.start (entries.front ().delayMs);
        }

        if (c) {
                startThread ();
        }
}

/*****************************************************************************/

bool Can::send (CanFrame const &buff) { return true; }

/*****************************************************************************/

CanFrame Can::read ()
{
        if (cnt >= entries.size () || !timer.isExpired ()) {
                simulateTimeout ();
        }

        CanFrame const &f = entries[cnt++].frame;

        if (cnt < entries.size ()) {
                timer.start (entries[cnt].delayMs);
        }

        return f;
}

/*****************************************************************************/

void Can::simulateTimeout () const { usleep (1000 * 50); }

/*****************************************************************************/

void Can::setCanCallback (ICanCallback *c)
{
        callback = c;

        if (c) {
                startThread ();
        }
}

/*****************************************************************************/

void Can::startThread ()
{
        thread = std::thread ([this] {
                if (!entries.empty ()) {
                        timer.start (entries.front ().delayMs);
                }

                while (cnt < entries.size ()) {
                        while (!timer.isExpired ()) {
                                usleep (100);
                        }

                        CanFrame const &f = entries[cnt++].frame;
                        callback->onCanNewFrame (f);

                        if (cnt < entries.size ()) {
                                timer.start (entries[cnt].delayMs);
                        }

                }
        });

        thread.detach ();
}
