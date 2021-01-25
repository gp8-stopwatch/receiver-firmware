## Crude test (test 6)
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

## Oscillator test (test 5)
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


## IR trigger + oscillator test (test 1)
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


Afetr some time (and changes in the source code) I've performed another test only to find out, that there is again somehing wrong:
10µs resolution 4s (TSSP 4056, 0.5ppm TCXO crystal). After implementing USB and fixing flash storage. 30cm apart.
| Timer 1 |
| ------- |
| 4,00096 |
| 4,00001 |
| 4,00001 |
| 4,00002 |
| 4,00002 |
| 4,00096 |
| 4,00003 |
| 4,00003 |
| 4,00004 |
| 4,00000 |
| 4,00001 |
| 4,00093 |
| 4,00002 |


## Absolute test
Testing against some stable source like (ntpd synchronized computer or some OCXO or even GPS disciplined OCXO like [this one](https://www.tindie.com/products/nsayer/gps-disciplined-ocxo/?pt=ac_prod_search). But they are relatively too expensive) over a course of, say, 5 minutes. External trigger GPIO.

## Temperature test
Absolute test in 0°C as well as in 50°C. Using the IR.

## CAN communication test (test 3)
There is a configuration possible where two receivers are connected together using CAN bus. Such a system works like that : 
1. A contestant starts crossing the first gate (ie. transmitter + receiver pair),
2. The first receiver immediately sends a signal to the second receiver and starts counting, updating its screen as the time passes.
3. The second receiver triggered by the signal received from the first receiver also starts counting (with display on).
4. A contestant finishes his/her course riding through the second gate, immediately triggering the second receiver off.
5. The second receiver sends the final result to the first receiver which stops its counter, discards its result, and displays the result of the second receiver.

Possible test : 
1. Increase result resolution to 1ms, 
2. let the second one be triggered by the first one, 
3. stop both at the same time using the external GPIO trigger,
4. observe the difference.

Initial results (100µs resolution). No code optimisations :

| Start timer | Finish timer |
| ----------- | -----------: |
| 04:00:00    |     03:99:81 |
| 04:00:00    |     03:99:08 |
| 04:00:00    |     03:99:88 |
| 04:00:00    |     03:99:78 |
| 04:00:00    |     03:99:69 |
| 04:00:00    |     03:99:59 |
| 04:00:00    |     03:99:49 |
| 04:00:00    |     03:99:39 |
| 04:00:00    |     03:99:30 |
| 04:00:00    |     03:99:20 |

Improvements that I've done.
1. Changed the trigger sensing from (initial and crude) polling/request buffering implementation to direct interrupts.
2. Changed the sending implementation from this request-for-send to direct function call.

Results after improvements show, that this was my lousy programming that caused the inacurracies. Cable length : 5m

| Start timer | Finish timer |
| ----------- | -----------: |
| 04:00:00    |     03:99:94 |

Cable length 0.8m (no difference whatsoever)

| Start timer | Finish timer |
| ----------- | -----------: |
| 04:00:00    |     03:99:94 |

Again the same test as above, but tthis time with 10µs resolution.

| Start timer | Finish timer |
| ----------- | -----------: |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99936 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99936 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99937 |
| 04:00:00    |     03:99936 |
| 04:00:00    |     03:99937 |

So measured latency is constant and equals about 630µs (probably a few microseconds more). The firmware takes it into account and corrects the result. Can bus latency has about 10µs accuracy (variation).

# Possible IR trigger tests
## Field test with slow-mo camera 
## Test with a motorized rig

