# The goal
It is very easy to demand too much from yourself... Especially when one dove into the rabbit hole, and dug into the topic for a long time striving for better and better results. So I have to establish a clear boundary and to know when to stop. 

* [ ] Superb : sub 1ppm - no error between 2 stop-watches running for 99 minutes, 99 seconds and 99 100th of a second (tested against a reference source as well).
* [ ] Satisfactory : 2ppm ?
* [ ] Absolute required : no difference on two random stopwatches running say 2 minutes with 10ms resolution (this is about 80ppm).

The above goals are important to be able to show off the product. The simplest test is to test them against each other.

# Factors affecting accuracy of measured time in GP8 stopwatch.

## Oscillator *Relative test, Absolute test*
This problem is quite straightforward, use the most accurate and stable resonator / oscillator there is, while maintaining the cost on a viable level. I'm in the process of testing ~~2ppm~~ 0.5ppm TCXOs now.

# Program
## Trigger algorithm 
How the timers are configured and used (TIM14 for stopwatch and TIM1 for trigger) how interrupts are configured and serviced etc.

Refresh rate of the screen started to have impact. Probably some bug in the code.

## Trigger
## IR Receiver component
Receiver speed / time resolution - I'm referring to the IR receiver component. Most of the receivers are suited for receiving fast and short bursts of IR signals of the carrier wave frequency like 38kHz. This is because they are all designed for remote control applications like TVs. Sucha a receiver would not only include a band pass filter to allow only the carrier in, but also a signal conditioning circuit which would additionally filter out any spurious bursts (of incorrect length). In other words, bursts have to conform to specs. For example, the minimum length of a burst in case of Sharp GP1UX51QS receiver was 600µs, and the maximum 1200µs (this is for a high-level pulse, for low-level pulse figures are quite similar). That means, that shining a continuous carrier signal would not work (this is a scenario best suited for my need). And an excerpt from the datasheet of aforementioned component : *In case the signal format of total duty and/or ON/OFF signal time dosen`t meet the conditions noted above, there is a case that reception distance much reduces or output dose not appear.* (sic).

If a continuous signal is bad, then (I assume) one should use the fastest 50% duty cycle signal possible. I went for 1kHz initially which translates to 500µs high, and 500µs low pulses, so this way we can have a 500µs error.

Initially, I went for Vishay's TSOP 38338. It also have very precise requirements for the signal (bear with me, I'm talking about the final bursts of 38khZ carrier not, the carrier itself). For instance, on page 5 of the datasheet they state, that :
* For bursts greater than 35 cycles, a minimum gap time in the data stream is needed of > 6 x burst length. 
* The maximum number of continuous short bursts/second : 2000. 
So seems like we can't use a continuous signal (my tests showed otherwise, at least sort of), and the other way around, we cannot use bursts more frequent than 2kHz which is another way of increasing trigger accuracy. But the most crucial parameter is the response time:

![Datasheet](doc/output-response-tsal-38338.png)

According to the datasheet (fig 1 page 2), td is between 3/f0 and 9/f0, in other words we can expect a response (low level) from 79µs to 237µs after an IR diode started to transmit a burst, and I assume the same goes for when a diode stops the transmission (which is interesting for us). So the error here is about 150µs in the worst case (for one receiver. For two, the total error can be as bad as 300µs). My crude observations showed it to be even smaller (the lag was 150µs on average spanning from about 125µs to 175µs). Note : the lag itself is not a huge problem since a stopwatch is stopped and started **usually** using the same trigger, so those lags cancel each other out, but the problem is a latency variance between them.

![My observations](doc/tsal-response.png)

[Application note](https://www.vishay.com/docs/82741/tssp4056sensor.pdf)
Next I performed some *rough* latency tests. Results:

| Lag (td) TSSP4056 µs | TSMP 58138  (6 pulses of 56kHz carrier)            | TSOP 38338                                                          |
| -------------------- | -------------------------------------------------- | ------------------------------------------------------------------- |
| 210                  | 110                                                |
| 130                  | 110                                                |
| 210                  | 110                                                |
| 210                  | 95                                                 |
| 210                  | 90                                                 |
| 200                  |                                                    |
| 210                  |                                                    |
| 130                  |                                                    |
| 140                  |                                                    |
| 140                  |                                                    |
| 140                  |                                                    |
| 210                  |                                                    |
| Measured : **80µs**  | **20µs** (theoretical not fouhn) but modulation is | measured **~40µs**, theoretical 160 @ 6cycles                       |
| Datasheet **142µs**  | required. 50% duty @ 56kHz is ~100µs,              | + modulation errors (~160) so in total IMHO 160+(160/2) = **240µs** |
|                      | so IMHO the total error is **90-110µs**            |

Latency depends on IR strength, so I


| Part                                   | Notes                                                                                                   |
| -------------------------------------- | ------------------------------------------------------------------------------------------------------- |
| TSMP58000                              | Minimal lag, small sensitivity (range 5m @ 400mA IR).                                                   |
| TSOP22xx, TSOP24xx, TSOP48xx, TSOP44xx | 7/f0 > td > 15/f0, range (range 45m @ 200mA IR)                                                         |
| TSOP312, TSOP314                       | 7/f0 > td > 15/f0, range (range 45m @ 200mA IR)                                                         |
| TSOP381xx, TSOP383xx, TSOP353xx        | 3/f0 > td > 9/f0, range (range 45m @ 200mA IR)                                                          |
| TSSP40xx                               | 7/f0 > td > 15/f0, range (range 25m @ 200mA IR) -                                                       |
| TSSP4056                               | specially for light barriers (38kHz & 56kHz), there are                                                 |
|                                        | no requirements for the pulses, so maybe better suited for                                              |
|                                        | continuous operation? Still 142µs lag dispersion @ 56kHz                                                |
|                                        | Application : Fast proximity sensors for toys, robotics, drones, and other consumer and industrial uses |
| TSOP21.., TSOP23.., TSOP41..,          | Simmilar to  TSOP38xxx                                                                                  |
| TSOP43.., TSOP25.., TSOP45.            |
| TSMP58138                              | For repeaters. There's no concreet figures, but from the                                                |
|                                        | picture I estimate the (constant part) of the lag to be  ~80µs.                                         |
|                                        | No error is given, so...                                                                                |
 
Vishay markings:
* O = for IR receiver applications
* M = for repeater/learning applications
* S = for sensor applications

Conclusions:
* Use what I use, but constant signal (test extensively). Understand the plots of this irradiance. It may be, that sensitivity drops when pulses are too long.
* UNDERSTAND IRRADIANCE PLOTS! These plots for TSOPs and TSSPs are distinct. It is as if TSOPs were less sensitive for long bursts where as TSSPs are unconcerned by the burst length. [Wikipedia says the irradiance](https://en.wikipedia.org/wiki/Irradiance) is also called an intensity.
* Maybe as above, but with 56kHz
* Test parts for light barriers.
* Test part for repeaters and IR code sniffers.

Links:
* https://electronics.stackexchange.com/questions/70811/ir-receivers-how-to-interpet-max-envelope-duty-cycle-vs-burst-length
* https://electronics.stackexchange.com/questions/70891/strange-and-contradictory-agc-characteristics-in-datasheet
* http://www.vishay.com/ir-receiver-modules/
* http://www.vishay.com/ir-receiver-modules/presence-sensor/


# Environment
  * IR intensity  
  * Reflections
  * Artificial light (fluorescent, LED)
  * Sunlight i ntensity (direct sun vs night)
  
# CAN connection
Methodology : 


# Possible tests
~~All tests presented here are not "unit-tests" i.e. they measure the whole system at once. This is done for simplicity sake~~

## Crude test 
Turning two identical receivers at the same time (using a transmitter operated by hand). These tests are only for crude estimation how the system performs.

Very first results (v3r2 with QCL8.00000F18B23B 20ppm, 8MHz crystal) show that there is a incaurracy with the trigger itself, because short lasting tests result in deviations in both directions (i.e. sometimes A is faster but sometimes B).
| Timer A  |                          Timer B |
| -------- | -------------------------------: |
| 3:23     |                             3:23 |
| 3:31     |                             3:31 |
| 3:43     |                             3:43 |
| 3:15     |                             3:15 |
| 3:25     |                         **3:26** |
| **3:21** |                             3:20 |
| 3:21     |                             3:21 |
| 3:38     |                             3:38 |
| 3:45     |                         **3:46** |
| 3:33     |                             3:33 |
| 3:51     |                         **3:52** |
|          |                                  |
| 10:97    |                            10:97 |
| 10:08    |                        **10:09** |
| 10:27    |                            10:27 |
| 10:29    |                            10:29 |
|          |                                  |
| 02:49:36 |                     **02:49:38** |
| 02:25:04 |                     **02:25:05** |
| 02:43:14 |                     **02:43:15** |
|          |                                  |
| 07:14:72 |  **07:14:75** (+0.007% = +70ppm) |
| 07:02:49 |  **07:02:51** (+0.005% = +50ppm) |
| 07:31:14 |            **07:31:17** (+70ppm) |
| 08:35:45 | ***08:35:50*** (0.01% = +100ppm) |

## Oscillator test
Turning two identical receivers at the same time (using a test GPIO trigger). This way we eliminate the IR trigger and we test (almost) only one thing at a time which is the oscillator accuracy in this case. Though there still is a part of code which drives the general-purpose timer, and a bit of state machine which decides whether to stop or start the stopwatch. The deeper through the rabbit hole, the more problems became apparent, read on.

Initial results (before optimizations):

10ms resolution, short term.
| Timer 1   |    Timer2 |
| --------- | --------: |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     | **04:01** |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| **04:01** |     04:00 |
| 04:00     |     04:00 |
| 04:00     | **04:01** |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     | **04:01** |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| 04:00     |     04:00 |
| **04:01** |     04:00 |
 

100µs resolution, short term.
| Timer 1  |   Timer2 |
| -------- | -------: |
| 04:00:04 | 04:00:05 |
| 04:00:04 | 04:00:07 |
| 04:00:04 | 04:00:07 |
| 04:00:03 | 04:00:07 |
| 04:00:03 | 04:00:06 |
| 04:00:02 | 04:00:04 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:05 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:08 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:02 | 04:00:06 |
| 04:00:01 | 04:00:05 |
| 04:00:02 | 04:00:06 |

After code optimizations the result was most of the time the same :
| Timer 1  |   Timer2 |
| -------- | -------: |
| 04:00:02 | 04:00:05 |


100µs resolution (*1s:*10ms:*100µs), long term (nominal is 59:00:00).
| Timer 1  |   Timer2 |
| -------- | -------: |
| 58:54:17 | 58:30:07 |
| 58:54:24 | 58:42:36 |
| 58:54:03 | 58:41:85 |
| 58:54:17 | 58:42:40 |
| 58:54:37 | 58:36:60 |
| 58:54:49 | 58:41:55 |
| 58:54:17 | 58:41:55 |
| 58:54:38 | 58:41:61 |
| 58:54:51 | 58:41:63 |
| 58:54:27 | 58:41:91 |


After code optimizations the result was always the same :
| Timer 1  |   Timer2 |
| -------- | -------: |
| 59:00:33 | 59:00:81 |


## IR trigger + oscillator test
As the previous test, but now the external trigger is connected to the transmitter instead of the receiver, thus testing the system as a whole (excluding the reflections from the environment and the beam interrupting object itself). These tests were performed after TIM14 configuration was improved.

100µs resolution 4s (TSOP 38338, 20ppm crystal).
| Timer 1  |   Timer2 |
| -------- | -------: |
| 03:00:99 | 04:00:02 |
| 04:00:01 | 04:00:04 |
| 03:00:99 | 04:00:03 |
| 04:00:04 | 04:00:07 |
| 04:00:05 | 04:00:08 |
| 04:00:08 | 04:00:01 |
| 04:00:07 | 04:00:11 |
| 03:00:97 | 04:00:00 |
| 04:00:04 | 04:00:07 |
| 04:00:06 | 04:00:09 |
| 04:00:02 | 04:00:06 |
| 04:00:00 | 04:00:03 |
| 04:00:02 | 04:00:06 |
| 04:00:01 | 04:00:05 |
| 04:00:04 | 04:00:07 |
| 03:00:97 | 04:00:01 |
| 04:00:01 | 04:00:05 |
| 04:00:03 | 04:00:05 |
| 04:00:01 | 04:00:05 |
| 04:00:00 | 04:00:04 |
| 04:00:06 | 04:00:09 |

100µs resolution 4s (TSSP 4056, 20ppm crystal).
| Timer 1  |       Timer2 |
| -------- | -----------: |
| 04:00:02 |     04:00:05 |
| 04:00:02 | **04:00:06** |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 | **04:00:06** |
| 04:00:02 |     04:00:05 |
| 04:00:02 | **04:00:06** |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |
| 04:00:02 |     04:00:05 |

100µs resolution 4s (TSSP 4056, 0.5ppm TCXO crystal).
| Timer 1 |   Timer2 |
| ------- | -------: |
| 04:00:0 | 04:00:00 | (10 times) |



Fastest Timer1 : 03:00:97s, slowest : 04:00:08s. Erorr x2 (start+stop) = 1.1ms, trigger error = +/- 0.55ms
Fastest Timer2 : 04:00:00s, slowest : 04:00:11s. Erorr x2 (start+stop) = 1.1ms, trigger error = +/- 0.55ms

100µs resolution, nominal 59s (TSOP 38338, 20ppm crystal).
| Timer 1  |   Timer2 |
| -------- | -------: |
| 59:00:31 | 59:00:79 |
| 59:00:33 | 59:00:81 |
| 59:00:37 | 59:00:85 |
| 59:00:30 | 59:00:78 |
| 59:00:33 | 59:00:81 |
| 59:00:33 | 59:00:81 |
| 59:00:38 | 59:00:86 |
| 59:00:30 | 59:00:79 |
| 59:00:33 | 59:00:81 |
| 59:00:30 | 59:00:78 |
| 59:00:32 | 59:00:81 |
| 59:00:32 | 59:00:80 |
| 59:00:32 | 59:00:80 |

Fastest Timer1 : 59:00:30s, slowest : 59:00:38s. Erorr x2 (start+stop) = 0.8ms, trigger error = +/- 0.4ms
Fastest Timer2 : 59:00:78s, slowest : 59:00:86s. Erorr x2 (start+stop) = 0.8ms, trigger error = +/- 0.4ms

100µs resolution, nominal 59s (TSSP 4056, 20ppm crystal).
| Timer 1  |       Timer2 |
| -------- | -----------: |
| 59:00:33 | **59:00:80** |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |
| 59:00:33 |     59:00:81 |

100µs resolution 59s (TSSP 4056, 0.5ppm TCXO crystal).
| Timer 1      |       Timer2 |
| ------------ | -----------: |
| **59:00:02** | **59:00:02** |
| **59:00:02** |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |
| 59:00:03     |     59:00:03 |

100µs resolution 99:99:99 (TSSP 4056, 0.5ppm TCXO crystal).
| Timer 1   |    Timer2 |
| --------- | --------: |
| 100:00:02 | 100:00:02 |
| 100:00:02 | 100:00:02 |
| 100:00:02 | 100:00:02 |



## Absolute test
Testing against some stable source like (ntpd synchronized computer or some OCXO or even GPS disciplined OCXO like [this one](https://www.tindie.com/products/nsayer/gps-disciplined-ocxo/?pt=ac_prod_search). But they are relatively too expensive) over a course of, say, 5 minutes. External trigger GPIO.

## Temperature test
Absolute test in 0°C as well as in 50°C. Using the IR.

## CAN communication test
There is a configuration possible where two receivers are connected together using CAN bus. Such a system works like that : 
1. A contestant starts crossing the first gate (ie. transmitter + receiver pair),
2. The first receiver immediately sends a signal to the second receiver and starts counting, updating its screen as the time passes.
3. The second receiver triggered by the signal received from the first receiver also starts counting (with display).
4. A contestant finishes his/her course riding through the second gate, immediately triggering the second receiver off.
5. The second receiver sends the final result to the first receiver which stops its counter, discards its result, and displays the result of the second receiver.

Possible test : 
1. Increase result resolution to 1ms, 
2. let the second one be triggered by the first one, 
3. stop both at the same time using the external GPIO trigger,
4. observe the difference.


# Possible IR trigger tests
## Field test with slow-mo camera 
## Test with a motorized rig

