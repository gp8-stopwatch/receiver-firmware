#SET (DEVICE "STM32F072xB")
SET (CUBE_ROOT "$ENV{HOME}/workspace/BlueNRG-1_2-DK-3.0.0")
#SET (CRYSTAL_HZ 8000000)
#SET (STARTUP_CODE "src/startup_stm32f072xb.s")
#SET (LINKER_SCRIPT "${CUBE_ROOT}/Project/BLE_Examples/Fixed_Stack/TrueSTUDIO/BlueNRG-2/BlueNRG2.ld")
SET (LINKER_SCRIPT "/home/iwasz/workspace/libmicro/scripts/BlueNRG2.ld")

SET (CMAKE_SYSTEM_NAME Generic)
SET (CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_C_FLAGS "-std=gnu99 -fdata-sections -ffunction-sections -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-std=c++17 -fno-rtti -fno-exceptions -fno-unwind-tables -Wall -fdata-sections -ffunction-sections -MD -Wall" CACHE INTERNAL "cxx compiler flags")
SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -static -Wl,-u,-Xlinker -Wl,--gc-sections" CACHE INTERNAL "exe link flags")


INCLUDE_DIRECTORIES(${SUPPORT_FILES})
LINK_DIRECTORIES(${SUPPORT_FILES})
#ADD_DEFINITIONS(-D${DEVICE})
#ADD_DEFINITIONS (-DHSE_VALUE=${CRYSTAL_HZ})
ADD_DEFINITIONS (-DBLUENRG2_DEVICE -DNRG2)

