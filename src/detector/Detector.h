/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Config.h"
#include "Types.h"
#include <cstdint>

/****************************************************************************/

enum class PwmState { low = 0, high = 1, middle };

/****************************************************************************/

#ifdef UNIT_TEST
inline auto &getConfig ()
{
        static cfg::Config config;
        return config;
}
#endif

/*****************************************************************************/

enum class EdgePolarity { falling = 0, rising = 1 };

/**
 * Signal edge with polarity and time of occurence.
 */
class Edge {
public:
        Edge () = default;
        Edge (Result1us const &ft) : msp{uint32_t (uint64_t (ft) >> 32)}, lsp{uint32_t (uint64_t (ft) & 0x000'0000'ffff'ffffULL)} {}

        Result1us getFullTimePoint () const { return (uint64_t (msp) << 32) | uint64_t (lsp); }
        Result1usLS getTimePoint () const { return lsp; }

private:
        uint32_t msp{};
        uint32_t lsp{};
};

/**
 * A slice of a rectangular signal.
 */
class EdgeQueue {
public:
        EdgeQueue (EdgePolarity firstPolarity) : firstPolarity{!bool (firstPolarity)} {}

        Edge &back () { return e2; }
        Edge &back1 () { return e1; }
        Edge &back2 () { return e0; }

        Edge &front () { return e0; }
        Edge &front1 () { return e1; }

        Edge &getE0 () { return e0; }
        Edge &getE1 () { return e1; }

        void push (Edge const &e)
        {
                e0 = e1;
                e1 = e2;
                e2 = e;
                firstPolarity = !firstPolarity;
        }

        Result1usLS getFirstDuration () const { return e1.getTimePoint () - e0.getTimePoint (); }
        Result1usLS getLastDuration () const { return e2.getTimePoint () - e1.getTimePoint (); }

        EdgePolarity getFirstPolarity () const { return EdgePolarity{firstPolarity}; }

private:
        Edge e0;
        Edge e1;
        Edge e2;
        bool firstPolarity{};
};

// Warning! Due to optimization reasons, the values below has to be in sync with enum Event
enum class DetectorEventType { trigger = 0, noise = 1, noNoise = 2, noBeam = 3, beamRestored = 4 };

// /// State of the IR beam that can be always queried.
// enum class DetectorStateType { ok, noise, noBeam };

/**
 *
 */
struct IEdgeDetectorCallback {
        virtual void report (DetectorEventType type, Result1us timePoint) = 0;
};
