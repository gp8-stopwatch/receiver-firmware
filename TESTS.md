# Factors affecting accuracy

* Oscillator *Relative test, Absolute test*
* Trigger
  * Algorithm
  * Receiver speed / time resolution
* Environment
  * IR intensity  
  * Reflections
  * Artificial light (fluorescent, LED)
  * Sunlight i ntensity (direct sun vs night)
* CAN connection

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

100µs resolution (*1s:*10ms:*100µs), long term.
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

