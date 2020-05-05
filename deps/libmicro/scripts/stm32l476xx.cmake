SET (DEVICE "STM32L476xx")
SET (CUBE_ROOT "$ENV{HOME}/workspace/STM32Cube_FW_L4_V1.13.0")
SET (STARTUP_CODE "${CUBE_ROOT}/Projects/NUCLEO-L476RG/Templates/TrueSTUDIO/startup_stm32l476xx.s")
SET (LINKER_SCRIPT "${CUBE_ROOT}/Projects/NUCLEO-L476RG/Templates/TrueSTUDIO/STM32L476RG_NUCLEO/STM32L476RG_FLASH.ld")

SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_C_FLAGS "-mcpu=cortex-m4 -std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-mcpu=cortex-m4 -std=c++17 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")
#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")
SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=rdimon.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

INCLUDE_DIRECTORIES(${SUPPORT_FILES})
LINK_DIRECTORIES(${SUPPORT_FILES})
ADD_DEFINITIONS(-D${DEVICE})

INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/STM32L4xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32L4xx/Include/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Include/")
