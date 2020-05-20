See [link link](http://) to see more.
Buy [on Tinidie](http://) TODO.

# Firmware upgrade
Manjaro : install dfu-util

```
dfu-util -D receiver.bin -d 0483:df11 -a0 --dfuse-address 0x08000000
```

git clone git@github.com:majbthrd/elf2dfuse.git
elf2dfuse receiver.elf receiver.dfu

dfu-util -D receiver.dfu -a0

# Key elements
```StopWatch``` class implements the time measurement. It uses TIM14 which runs at 100Hz, 1kHz or 10kHz depending on ```Config.resolution```. Its ```start``` method clears the ```TIM14->CNT``` (timer runs all the time), and sets a variable so that we know that tiome measurement is taking place. Upon ```stop``` call remaining value from ```TIM14->CNT``` is rounded and basing on this 1 is added to the ```time``` field.

```InfraRedBeamModulated```

```FastStateMachine```


# Features
* X ppm accuracy
* Screen brightness auto adjusts according to a ambient light sensor.

Maximum error between the devices should not exceed 2*20ppm (worst case).

# Challenges
* Accuracy

Bob's take on moto-gymkhana stop watch : https://github.com/boons605/MotoGymkhanaRaceTiming


# Libraries, and 3rd party sources used:
* https://github.com/majbthrd/stm32cdcuart
* https://github.com/antirez/linenoise (?)
* STM32F0 cube
* etl
* GSL
  