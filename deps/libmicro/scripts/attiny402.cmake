SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR avr)

SET(CMAKE_C_FLAGS "-specs=/home/iwasz/Downloads/Atmel.ATtiny_DFP.1.3.229/gcc/dev/attiny402/device-specs/specs-attiny402 -mmcu=avrxmega3 -std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-specs=/home/iwasz/Downloads/Atmel.ATtiny_DFP.1.3.229/gcc/dev/attiny402/device-specs/specs-attiny402 -mmcu=avrxmega3 -std=c++11 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")

ADD_DEFINITIONS ("-D__AVR_ATmega2560__")
