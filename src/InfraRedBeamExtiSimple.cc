/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Container.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "FastStateMachine.h"
#include "InfraRedBeamExti.h"
#include "StopWatch.h"

/*****************************************************************************/

void sendEvent (FastStateMachine *fStateMachine, Event ev) { fStateMachine->run (ev); }

/*****************************************************************************/

// Should be called only on the TR falling edge.
void InfraRedBeamExti::onExti (IrBeam state, bool external)
{
        Result now = stopWatch->getTime ();

        if (lastIrChangeTimePoint > 0 && !triggerRisingEdgeTime) {
                triggerRisingEdgeTime = now - lastIrChangeTimePoint;
        }

        lastIrChangeTimePoint = now;
}

/*****************************************************************************/

void InfraRedBeamExti::run ()
{
        if (triggerRisingEdgeTime) {
                sendEvent (fStateMachine, {Event::Type::irTrigger, *triggerRisingEdgeTime});
                triggerRisingEdgeTime.reset ();
        }
}
