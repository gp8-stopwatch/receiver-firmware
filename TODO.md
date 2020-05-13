# Product
* [ ] Make 10
* [ ] Put on sale

# Possible features
* [ ] µC in the transmitter
  * [ ] Battery sensing in the transmitter
* [ ] Hardware battery protection in both devices
* [ ] Test trigger in the transmitter (same as receiver)
* [ ] Speed trap. Receivers connected together would measure the speed.

# Accuracy
* [ ] Make sure fw is compiled with -O3 (trigger algorithm)
* [ ] Make unit-tests possible. GPIO external trigger.
  * [ ] Make USB output with 1m resolution - for trigger testing.
* [ ] Use TXCO obviously. Pick one and change the footprint.
  * [x] Choose one
  * [ ] Test it somehow (it's tiny, and I don't have a footprint on PCB)
* [ ] There has to be a bug in the trigger algorithm. When the resolution was set to 10ms there have been off by 10ms errors on 4s runs (1 in 20 aprox). But when the resolution was set to 100µs, the error fluctuates between 300 and 600µs (-O0). When -O3 and 100µs resolution, the error is smaller like 200-400µs but it also fluctuates. 10ms = 10000µs. Divided by 20 gives 500µs, so it is almost like in the first scenario the errors accumulated somehow. 
* [x] Timer TIM1 should be restarted (counting from 0) when start event happens.
  * [x] test
* [x] Value of the counter should be taken into account when system stops. It's value should be rounded, not discarded like now.
  * [x] test
* [x] Trigger tests
* [ ] Improve IR trigger
  * [ ] Test new receivers (for IR curtains, barriers)
  * [ ] 

# Hardware
* [x] Boot pin easy accessible (for DFU).
* [ ] Crystal resonator footprint suitable for modern resonators, not this THT crap.
* [ ] Reduce holes for CAN socket supports (thise 2 plastic one sticking out)
* [ ] Ldo for etc should be 3v3 not 1v8. Voltage drop would be smaller
* [ ] Battery protection in software
  * [ ] When powered off no software is running. What is the current draw of ldo plus rtc?
  * [ ] Measure what all elements except the µC and LEDs are drawing, and whether we should optimize this, or leave alone.
  * [ ] When powered on, simply go to sleep, calculate current, maybe modify HW so it draws less quiescent current.
  * [ ] Same for transmitter
* [ ] If the transmitter had an uc, we could use the charging led to indicate that it is running, and there would be no changes to the casing necessary.
* [ ] Charging led to uc, charger outputs as well. This way we could sense when charger plugged in.

# GUI 
* [ ] On screen menu
  * [ ] Brightness (?)
  * [ ] 1 or 2 participants
  * [ ] Loop mode / normal
* [ ] USB menu should include all what on-display provides plus:
  * Date / time
  * Results

# Other
* [ ] Settings memory (screen flip & sound on/off should be persisted)
* [x] USB firmware upgrade
  * [x] Test
  * [ ] Document
* [ ] USB CDC. There are huge problems.
* [ ] RTC
  * [x] Write time and date to the output
  * [ ] Rtc date and time is persiusted when power is turned off
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

