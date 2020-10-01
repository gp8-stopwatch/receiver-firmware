CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE OFF)
SET (CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

PROJECT (receiver)
ENABLE_LANGUAGE (ASM-ATT)

INCLUDE_DIRECTORIES("src/")
INCLUDE_DIRECTORIES("src/usb")
INCLUDE_DIRECTORIES("deps/etl/include/")
INCLUDE_DIRECTORIES("deps/GSL/include/")
INCLUDE_DIRECTORIES("deps/STM32F0xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("deps/CMSIS/Device/ST/STM32F0xx/Include/")
INCLUDE_DIRECTORIES("deps/CMSIS/Device/ST/STM32F0xx/Include/Legacy")
INCLUDE_DIRECTORIES("deps/CMSIS/Include/")
INCLUDE_DIRECTORIES("deps/simple-cli/src")
INCLUDE_DIRECTORIES("deps/libstate/src")

SET (LIB_MICRO_ROOT "deps/libmicro")
INCLUDE_DIRECTORIES ("${LIB_MICRO_ROOT}/src")

# Regular receiver with all the features
set(WITH_SOUND ON CACHE BOOL "Compile buzzer support")
set(WITH_DISPLAY ON CACHE BOOL "Compile 7 segment LED display support")
set(WITH_MENU OFF CACHE BOOL "Menu support. Disabling it considerably reduces binary size in debug mode.")
set(WITH_COMMON_ANODE OFF CACHE BOOL "ON means common anode 7 segments, OFF means common cathode")
set(WITH_USB OFF CACHE BOOL "USB serial smulation support")
set(WITH_FLASH ON CACHE BOOL "Store settings and results to flash")
set(WITH_HISTORY OFF CACHE BOOL "Store settings and results to flash")
set(WITH_CAN ON CACHE BOOL "Complie CAN bus support")
set(WITH_TEST_TRIGGER_MOD_2 OFF CACHE BOOL "When ON test trigger is deaf every second time. This is for testing synchornisation between 2 receivers.")
set(FLASH_QUEUE_SIZE 4 CACHE STRING "Results are stored into protected queue from an ISR. This is the size of this queue.")
set(WITH_RTC ON CACHE BOOL "Complie RTC support")
set(WITH_POWER_MANAGER ON CACHE BOOL "Complie power sensing support")
set(WITH_CHECK_SENSOR_STATUS ON CACHE BOOL "If on, the device checks for external sensors and queries their state")
set(WITH_BUTTON ON CACHE BOOL "Compile button support")
ADD_DEFINITIONS (-DPLATFORM_REGULAR)

# Micro receiver
# set(WITH_SOUND OFF CACHE BOOL "Compile buzzer support")
# set(WITH_DISPLAY OFF CACHE BOOL "Compile 7 segment LED display support")
# set(WITH_MENU OFF CACHE BOOL "Menu support. Disabling it considerably reduces binary size in debug mode.")
# set(WITH_COMMON_ANODE OFF CACHE BOOL "ON means common anode 7 segments, OFF means common cathode")
# set(WITH_USB OFF CACHE BOOL "USB serial smulation support")
# set(WITH_FLASH OFF CACHE BOOL "Store settings and results to flash")
# set(WITH_HISTORY ON CACHE BOOL "Store settings and results to flash")
# set(WITH_CAN ON CACHE BOOL "Complie CAN bus support")
# set(WITH_TEST_TRIGGER_MOD_2 OFF CACHE BOOL "When ON test trigger is deaf every second time. This is for testing synchornisation between 2 receivers.")
# set(FLASH_QUEUE_SIZE 4 CACHE STRING "Results are stored into protected queue from an ISR. This is the size of this queue.")
# set(WITH_RTC OFF CACHE BOOL "Complie RTC support")
# set(WITH_POWER_MANAGER OFF CACHE BOOL "Complie power sensing support")
# set(WITH_CHECK_SENSOR_STATUS OFF CACHE BOOL "If on, the device checks for external sensors and queries their state")
# set(WITH_BUTTON ON CACHE BOOL "Compile button support")
# ADD_DEFINITIONS (-DPLATFORM_MICRO)

# Huge display without built in receiver
# set(WITH_SOUND ON CACHE BOOL "Compile buzzer support")
# set(WITH_DISPLAY ON CACHE BOOL "Compile 7 segment LED display support")
# set(WITH_MENU OFF CACHE BOOL "Menu support. Disabling it considerably reduces binary size in debug mode.")
# set(WITH_COMMON_ANODE ON CACHE BOOL "ON means common anode 7 segments, OFF means common cathode")
# set(WITH_USB ON CACHE BOOL "USB serial smulation support")
# set(WITH_FLASH ON CACHE BOOL "Store settings and results to flash")
# set(WITH_HISTORY ON CACHE BOOL "Store settings and results to flash")
# set(WITH_CAN ON CACHE BOOL "Complie CAN bus support")
# set(WITH_TEST_TRIGGER_MOD_2 OFF CACHE BOOL "When ON test trigger is deaf every second time. This is for testing synchornisation between 2 receivers.")
# set(FLASH_QUEUE_SIZE 4 CACHE STRING "Results are stored into protected queue from an ISR. This is the size of this queue.")
# set(WITH_RTC ON CACHE BOOL "Complie RTC support")
# set(WITH_POWER_MANAGER ON CACHE BOOL "Complie power sensing support")
# set(WITH_CHECK_SENSOR_STATUS ON CACHE BOOL "If on, the device checks for external sensors and queries their state")
# set(WITH_BUTTON ON CACHE BOOL "Compile button support")
# ADD_DEFINITIONS (-DPLATFORM_HUGE)

IF (WITH_SOUND)
    ADD_DEFINITIONS (-DWITH_SOUND=1)
ENDIF()

IF (WITH_DISPLAY)
    ADD_DEFINITIONS (-DWITH_DISPLAY=1)
ENDIF()

IF (WITH_COMMON_ANODE)
    ADD_DEFINITIONS (-DCOMMON_ANODE=1)
ELSE ()
    ADD_DEFINITIONS (-DCOMMON_CATHODE=1)
ENDIF ()

IF (WITH_USB)
    ADD_DEFINITIONS (-DWITH_USB=1)
ENDIF ()

IF (WITH_FLASH)
    ADD_DEFINITIONS (-DWITH_FLASH=1)
ENDIF ()

IF (WITH_HISTORY)
    ADD_DEFINITIONS (-DWITH_HISTORY=1)
ENDIF ()

IF (WITH_CAN)
    ADD_DEFINITIONS (-DWITH_CAN=1)
ENDIF ()

IF (WITH_TEST_TRIGGER_MOD_2)
    ADD_DEFINITIONS (-DTEST_TRIGGER_MOD_2=1)
ENDIF ()

ADD_DEFINITIONS (-DFLASH_QUEUE_SIZE=${FLASH_QUEUE_SIZE})

IF (WITH_MENU)
    ADD_DEFINITIONS (-DWITH_MENU=1)
ENDIF ()

IF (WITH_RTC)
    ADD_DEFINITIONS (-DWITH_RTC=1)
ENDIF ()

IF (WITH_POWER_MANAGER)
    ADD_DEFINITIONS (-DWITH_POWER_MANAGER=1)
ENDIF ()

IF (WITH_CHECK_SENSOR_STATUS)
    ADD_DEFINITIONS (-DWITH_CHECK_SENSOR_STATUS=1)
ENDIF ()

IF (WITH_BUTTON)
    ADD_DEFINITIONS (-DWITH_BUTTON=1)
ENDIF ()

# DEVICE is defined in the toolchain file, so if it's present, we assume that the firmware is beeing built.
IF (DEFINED DEVICE)
    SET(SOURCES
        "src/main.cc"
        "src/Button.cc"
        "src/FastStateMachine.cc"
        "src/InfraRedBeamExti.cc"
        "src/StopWatch.cc"
        "src/Config.cc"
        "src/Container.cc"
        "src/DisplayMenu.cc"

        "${LIB_MICRO_ROOT}/src/ErrorHandler.c"
        "${LIB_MICRO_ROOT}/src/TimerF.cc"
        "${LIB_MICRO_ROOT}/src/Debug.cc"
        "${LIB_MICRO_ROOT}/src/GpioF.cc"
        "${LIB_MICRO_ROOT}/src/GpioF0.cc"
        "${LIB_MICRO_ROOT}/src/UsartF.cc"
        "${LIB_MICRO_ROOT}/src/UsartF0.cc"

        "${STARTUP_CODE}"
        "src/stm32f0xx_it.c"
        "src/system_stm32f0xx.c"
      
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c"
        "deps/STM32F0xx_HAL_Driver/Src/Legacy/stm32f0xx_hal_can.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart_ex.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_usart.c"
        "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usb.c"
    )

    IF (WITH_DISPLAY)
        LIST (APPEND SOURCES 
        "${LIB_MICRO_ROOT}/src/HardwareTimer.cc"
        "src/Led7SegmentDisplay.cc"
        )
    ENDIF ()

    IF (WITH_USB)
        LIST (APPEND SOURCES
        "src/usb/usbd_cdc.c"
        "src/usb/usbd_composite.c"
        "src/usb/usbd_conf.c"
        "src/usb/usbd_core.c"
        "src/usb/usbd_ctlreq.c"
        "src/usb/usbd_desc.c"
        "src/usb/usbd_ioreq.c"
        "src/usb/UsbHelpers.cc"
        )        
    ENDIF ()

    IF (WITH_CAN)
        LIST (APPEND SOURCES
        "${LIB_MICRO_ROOT}/src/CanF.cc"
        "${LIB_MICRO_ROOT}/src/CanFrame.cc"
        "src/CanProtocol.cc"
        )
    ENDIF ()

    IF (WITH_RTC)
        LIST (APPEND SOURCES
        "src/Rtc.cc"
        )
    ENDIF ()

    IF (WITH_SOUND)
        LIST (APPEND SOURCES
        "src/Buzzer.cc"
        )
    ENDIF ()

    IF (WITH_HISTORY)
        LIST (APPEND SOURCES
        "src/History.cc"
        )
    ENDIF ()

    IF (WITH_POWER_MANAGER)
        LIST (APPEND SOURCES
        "src/PowerManagement.cc"
        )
    ENDIF ()

    ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf ${SOURCES})

    FIND_PROGRAM (OPENOCD openocd)
    ADD_CUSTOM_TARGET("upload" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg -c 'program ${CMAKE_PROJECT_NAME}.elf verify reset exit')
    ADD_CUSTOM_TARGET("optionbytes" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg -c 'init; mww 0x40022004 0x45670123; mww 0x40022004 0xCDEF89AB; mww 0x40022008 0x45670123; mww 0x40022008 0xCDEF89AB; mww 0x40022010 0x00000220; mww 0x40022010 0x00000260; mww 0x40022010 0x00000210; mwh 0x1ffff800 0x55AA; mwh 0x1ffff802 0x807f; mdw 0x1ffff800; exit')
    ADD_CUSTOM_TARGET("debug" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg)
    ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.bin ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)

    FIND_PROGRAM (ELF2DFUSE elf2dfuse)
    ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.dfu ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${ELF2DFUSE} ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.dfu)
ELSE()
    INCLUDE_DIRECTORIES("src/test")

    SET (CATCH2_INCLUDE "deps/Catch2/single_include/catch2")
    INCLUDE_DIRECTORIES("${CATCH2_INCLUDE}")

    ADD_DEFINITIONS (-DUNIT_TEST=1)

    # TEST is beeing built
    ADD_EXECUTABLE(test
        "src/test/00CatchInit.cc"
        "src/test/02Itoa.cc"
    )

ENDIF ()
