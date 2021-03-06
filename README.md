See [link link](http://) to see more.
Buy [on Tinidie](http://) TODO.

# How to set up
* Turn the transmitter off, and receiver on.
* Receiver should show "no i.r.". It that's not the case, the sunlight, or fluorescent light might interfere with it. Avoid placing the receiver so the sensor faces the sun or other intense source of light.

# Firmware upgrade
See docs.iwasz.com


# Key elements
```StopWatch``` class implements the time measurement. It uses TIM14 which runs at 100Hz, 1kHz or 10kHz depending on ```Config.resolution```. Its ```start``` method clears the ```TIM14->CNT``` (timer runs all the time), and sets a variable so that we know that tiome measurement is taking place. Upon ```stop``` call remaining value from ```TIM14->CNT``` is rounded and basing on this 1 is added to the ```time``` field.

```InfraRedBeamExti```

```FastStateMachine```


# Features
* X ppm accuracy
* Screen brightness auto adjusts according to a ambient light sensor.

Maximum error between the devices should not exceed 2*20ppm (worst case).

# Challenges
* Accuracy

# Links
* Bob's take on moto-gymkhana stop watch : https://github.com/boons605/MotoGymkhanaRaceTiming
* Pyxis's tripod : https://www.thingiverse.com/thing:3284642

# Libraries, and 3rd party sources used:
* https://github.com/majbthrd/stm32cdcuart
* https://github.com/antirez/linenoise (?)
* STM32F0 cube
* etl
* GSL
  
# Serial connection 
See docs.iwasz.com

# NVIC pins
IR PA8
Button PB15
Test trigger PB3

# Current draw when powered on (by RTC & LDO)
1.9 - 2.5 µA

# Valgrind
```sh
valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./profile 
```

FAQ
* Noise error : check terminator switches. Check if cables are connected on both sides.

# Kolejność motażu
## Receiver
1. SMD
1. µC
2. USB, LDO low Q, TCXO
3. LED RG. Kołnież nie może wystawać poza PCB i jak najniżej
4. kolejne THT
5. Wyświetlacze (uwaga na orientację, uwaga na CA-CC)
6. Ewentualnie przetestować 1 wyświetlacz
7. Battery holder:
   1. Pobielić pady i styki
   2. Hot glue
   3. Zlutować
8. IR

## Micro
1. SMD
2. THT