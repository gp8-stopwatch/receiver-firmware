# Typical configurations
Receiver and transmitter.

Receiver + sensor with two transmitters.

Receiver (IR off) + two sensors and two transmitters.


# Blind state
Is managed by the `BlindManager` class. Its `start` method starts the so called blind period when the (main) device does not react to any events. To remember:
* "Micro" sensors does not have this functionality built in. They always send a pulse on the LVDS.
* Tha main receiver has a global `BlindManager` which state is controlled by:
  * `FastStateMachine` chekcs if not blind and starts the blind period upon receiveing `Event::Type::irTrigger` or `Event::Type::externalTrigger`.
  * `IrTriggerDetector` (formely known as the `EdgeDetector`) checks if not blind in both `onEdge` and run `methods` to prevent spurious state changes in the object. Without this safety check, there could have been false triggers reported to the `FastStateMachine` after the blind period expired.

The rationale behind this is to maintain single global "blind timer" managed by the main device, meaning that every gate (i.e. light barier) is disabled during this period (technically it is not, but the main device does not react).

# CAN Bus
50kbps, only for communications. No time related stuff.


