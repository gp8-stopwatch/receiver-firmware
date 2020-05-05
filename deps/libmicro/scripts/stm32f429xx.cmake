SET (DEVICE "STM32F429xx")
SET (CUBE_ROOT "$ENV{HOME}/workspace/STM32Cube_FW_F4_V1.16.0")
SET (STARTUP_CODE "$ENV{HOME}/workspace/libmicro/scripts/startup_stm32f429xx.s")
SET (LINKER_SCRIPT "$ENV{HOME}/workspace/libmicro/scripts/STM32F429ZI_FLASH.ld")


SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_C_FLAGS "-std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-std=c++17 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")
#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=rdimon.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")
SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

INCLUDE_DIRECTORIES(${SUPPORT_FILES})
LINK_DIRECTORIES(${SUPPORT_FILES})
ADD_DEFINITIONS(-D${DEVICE})
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Include/")
