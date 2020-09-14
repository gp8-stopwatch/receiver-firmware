/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Types.h"
#include <cstdint>

/// State of the IR beam (light).
enum class IrBeam { absent, present, noise };

/**
 *Base for various receivers.
 */
struct IInfraRedBeam {
        virtual IrBeam getBeamState () const = 0;
        // virtual bool isBeamPresent () const = 0;
        virtual bool isBeamInterrupted () const = 0;

        virtual bool isActive () const = 0;
        virtual void setActive (bool b) = 0;
};
