# Product
* [ ] Make 10
* [ ] Put on sale

# Possible features
* µC in the transmitter
  * Battery sensing in the transmitter
* Hardware battery protection in both devices
* Test trigger in the transmitter (same as receiver)
* Speed trap. Receivers connected together would measure the speed.

# Accuracy
* [ ] Make sure fw is compiled with -O3 (trigger algorithm)
* [ ] Make unit-tests possible. GPIO external trigger.
  * [ ] Make USB output with 1m resolution - for trigger testing.
* [ ] Use TXCO obviously. Pick one and change the footprint.
* [ ] There has to be a bug in the trigger algorithm. When the resolution was set to 10ms there have been off by 10ms errors on 4s runs (1 in 20 aprox). But when the resolution was set to 100µs, the error fluctuates between 300 and 600µs (-O0). When -O3 and 100µs resolution, the error is smaller like 200-400µs but it also fluctuates. 10ms = 10000µs. Divided by 20 gives 500µs, so it is almost like in the first scenario the errors accumulated somehow. 
* [x] Timer should be restarted (counting from 0) when start event happens.
  * [ ] test
* [x] Value of the counter should be taken into account when system stops. It's value should be rounded, not discarded like now.
  * [ ] test

# Hardware
* [x] Boot pin easy accessible (for DFU).
* [ ] Crystal resonator footprint suitable for modern resonators, not this THT crap.
* [ ] Reduce holes for CAN socket supports (thise 2 plastic one sticking out)

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
* [ ] USB firmware upgrade
  * [ ] Test
  * [ ] Document

# Infrastructure
* [x] Clean the code (prepare for new repo that is)
  * [x] Code should be self-contained. Everything needed for building should be included.
* [x] Remove this "big" display project for now.
* [x] Prepare and commit other projects
* [ ] Release FW as opensource.
* [ ] Prepare a web page with documentation.

 * [ ] battery level sensing.
 * [ ] loop measurements
 * [ ] input in console
 * [ ] RTC
 * [ ] 2 or 3? contestants
 * [ ] Time bigger than 16b in history and everywhere else.
 * [ ] EWENTUALNIE demko . Cyfry pokazują się od prawej. Najpierw segmenty 1, potem do 0, potem 8. Wszystkie 6. Potem znów od prawej znikają. Tak było w
 * Fz1, Fz6 i Xj6
 * [ ] Wyświetlanie zegara.
 * [ ] Kiedy nie ma IR, to wyświetlać same kreski, albo -no ir-
 * [ ] Optimize spaghetti code in the FastState machine
 * [x] LED multiplexing driven by hardware timer to prevent frying it in case of program hang.
 * [x] When other CAN devices are absent, we should deal with it gracefully. Now I throw hundreds of error messages driving system useless.
 * [x] buzzer volume or if buzzer at all.
 * [x] Screen dims itself to 0 sometimes.