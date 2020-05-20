# Product
* [ ] Make 10
* [ ] Put on sale

# Possible features
* [ ] µC in the transmitter
  * [ ] Battery sensing in the transmitter
* [ ] Hardware battery protection in both devices
* [ ] Test trigger in the transmitter (same as receiver) - a test point(s)
* [ ] Speed trap. Receivers connected together would measure the speed.
* [ ] Acurracy setting (100µs)

# Accuracy
* [ ] Make sure fw is compiled with -O3 (trigger algorithm)
* [x] Make unit-tests possible. GPIO external trigger.
  * [ ] Make USB output with 1m resolution - for trigger testing.
* [x] Use TXCO obviously. Pick one and change the footprint.
  * [x] Choose one
  * [x] Test it somehow (it's tiny, and I don't have a footprint on PCB)
* [x] There has to be a bug in the trigger algorithm. When the resolution was set to 10ms there have been off by 10ms errors on 4s runs (1 in 20 aprox). But when the resolution was set to 100µs, the error fluctuates between 300 and 600µs (-O0). When -O3 and 100µs resolution, the error is smaller like 200-400µs but it also fluctuates. 10ms = 10000µs. Divided by 20 gives 500µs, so it is almost like in the first scenario the errors accumulated somehow. 
* [x] Timer TIM1 should be restarted (counting from 0) when start event happens.
  * [x] test
* [x] Value of the counter should be taken into account when system stops. It's value should be rounded, not discarded like now.
  * [x] test
* [x] Trigger tests
* [x] Improve IR trigger
  * [x] Test new receivers (for IR curtains, barriers)
* [ ] CAN latency tests.
* [ ] GPSDO tests

# Hardware
* [x] Boot pin easy accessible (for DFU).
* [x] Crystal oscillator footprint suitable for modern TCXOs, not this THT crap.
* [ ] ~~Reduce holes for CAN socket supports (thise 2 plastic one sticking out)~~ The case is holding the socket in place. Too much effort.
* [x] Ldo for etc should be 3v3 not 1v8. Voltage difference would be smaller.
* [ ] Przetestować RTC z tym małym LDO.
* [ ] Battery protection in software
  * [ ] When powered off no software is running. What is the current draw of ldo plus rtc?
  * [ ] Measure what all elements except the µC and LEDs are drawing, and whether we should optimize this, or leave alone.
  * [ ] When powered on, simply go to sleep, calculate current, maybe modify HW so it draws less quiescent current.
  * [ ] Same for the transmitter
* [ ] If the transmitter had an uc, we could use the charging led to indicate that it is running, and there would be no changes to the casing necessary.
* [x] ~~Charging led to uc,~~ charger outputs as well. This way we could sense when charger plugged in.
* [ ] Check common anode configuration
* [x] Check µC - charger connection.
* [ ] Field tests of the trigger 
  * [x] Rough range tests shows that there's no noticeable change.
* [x] 56kHz transmitter.
* [x] Test points.
* [ ] ~~Fix the distance between the holes on 18650 battery holder. **Not until new arrive**.~~ Besides : the holder is supported by case's back wall.
* [x] Route the receiver PCB
* [ ] Poprawić działanie we wnętrzach (incadescent lights & cameras) ????
* [ ] Jeszcze osłabić sygnał IR.

# Case
* [ ] Make sure the holder is well supported by case's back wall. It has to be fixed in place. This way we can ignore the screws that would normally fix the holder to tyhe PCB.
* [ ] Pod grilem musi być wytłoczenie, bo głośniczek jest za wysoki.
* [ ] Fotorezystor musi mieć lepszy dostęp do światła.
* [ ] 

# GUI 
* [ ] On screen menu
  * [ ] Brightness (1/2/3/auto). Auto as default.
  * [ ] 1 or 2 participants
  * [ ] Loop mode / normal
  * [ ] Date / time setting
* [ ] USB menu should include all what on-display provides plus:
  * Results

# Other
* [ ] Settings memory (screen flip & sound on/off should be persisted)
* [x] USB firmware upgrade
  * [x] Test
  * [ ] Document
* [ ] USB CDC. There are huge problems.
* [ ] RTC
  * [x] Write time and date to the output
  * [x] Rtc date and time is persisted when power is turned off
  * [ ] test with low quiescent LDO.
 * [ ] battery level sensing.
 * [ ] loop measurements 
 * [ ] input in console
 * [ ] 2 or 3? contestants
 * [ ] Time bigger than 16b in history and everywhere else.
 * [ ] Wyświetlanie zegara.
 * [ ] Kiedy nie ma IR, to wyświetlać same kreski, albo -no ir-
 * [ ] Optimize spaghetti code in the FastState machine
 * [x] LED multiplexing driven by hardware timer to prevent frying it in case of program hang.
 * [x] When other CAN devices are absent, we should deal with it gracefully. Now I throw hundreds of error messages driving system useless.
 * [x] buzzer volume or if buzzer at all.
 * [x] Screen dims itself to 0 sometimes.
  
# Infrastructure
* [x] Clean the code (prepare for new repo that is)
  * [x] Code should be self-contained. Everything needed for building should be included.
* [x] Remove this "big" display project for now.
* [x] Prepare and commit other projects
* [ ] Release FW as opensource.
* [ ] Prepare a web page with documentation.

# USB
Works under windows 7 and an old Ubuntu. Does not work under Manjaro `Linux futureboy 5.4.39-1-MANJARO #1 SMP PREEMPT Wed May 6 10:36:44 UTC 2020 x86_64 GNU/Linux`

Under Manjaro it behaves very unstable. Only 1 byte at a time san be sent to tyhe PC. if I try to sen 2B in a row, I get nothing on the serial console. If the device is connected through a USB hub, and I dosconnect it, I get the following dmesgs:


```
[ 4586.490066] xhci_hcd 0000:00:14.0: WARN Cannot submit Set TR Deq Ptr
[ 4586.490068] xhci_hcd 0000:00:14.0: A Set TR Deq Ptr command is pending.
[ 4586.492081] xhci_hcd 0000:00:14.0: WARN Cannot submit Set TR Deq Ptr
[ 4586.492084] xhci_hcd 0000:00:14.0: A Set TR Deq Ptr command is pending.
[ 4586.494061] xhci_hcd 0000:00:14.0: WARN Cannot submit Set TR Deq Ptr
[ 4586.494064] xhci_hcd 0000:00:14.0: A Set TR Deq Ptr command is pending.
[ 4586.496077] xhci_hcd 0000:00:14.0: WARN Cannot submit Set TR Deq Ptr
[ 4586.496079] xhci_hcd 0000:00:14.0: A Set TR Deq Ptr command is pending.
[ 4586.498067] xhci_hcd 0000:00:14.0: WARN Cannot submit Set TR Deq Ptr
[ 4586.498070] xhci_hcd 0000:00:14.0: A Set TR Deq Ptr command is pending.
[ 4586.498363] usb 2-4.2.2: USB disconnect, device number 12
[ 4586.500122] cdc_acm 2-4.2.2:1.0: failed to set dtr/rts
```

Solved. The problem was due to lack of ...