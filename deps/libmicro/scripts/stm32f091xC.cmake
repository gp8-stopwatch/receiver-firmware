SET (DEVICE "STM32F091xC")
SET (CUBE_ROOT "$ENV{HOME}/workspace/STM32Cube_FW_F0_V1.7.0")
SET (CRYSTAL_HZ 16000000)
SET (STARTUP_CODE "$ENV{HOME}/workspace/libmicro/scripts/startup_stm32f091xc.s")
SET (LINKER_SCRIPT "$ENV{HOME}/workspace/libmicro/scripts/STM32F091CBU6_FLASH.ld")


SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET (CMAKE_C_FLAGS "-std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-std=c++17 -fno-rtti -fno-exceptions -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")
#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=rdimon.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")
SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

INCLUDE_DIRECTORIES(${SUPPORT_FILES})
LINK_DIRECTORIES(${SUPPORT_FILES})
ADD_DEFINITIONS(-D${DEVICE})
ADD_DEFINITIONS (-DHSE_VALUE=${CRYSTAL_HZ})
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32F0xx/Include/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Drivers/CMSIS/Include/")
