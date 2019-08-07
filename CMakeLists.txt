CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE OFF)

PROJECT (receiver-led-small)
ENABLE_LANGUAGE (ASM-ATT)

INCLUDE_DIRECTORIES("src/")
INCLUDE_DIRECTORIES("deps/libmicro/src/")
INCLUDE_DIRECTORIES("deps/libstopwatch/")
INCLUDE_DIRECTORIES("deps/etl/include/")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc")
INCLUDE_DIRECTORIES("${CUBE_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc")

SET (LIB_MICRO_ROOT "deps/libmicro")
INCLUDE_DIRECTORIES ("${LIB_MICRO_ROOT}/src")

SET (LIB_STOPWATCH_ROOT "deps/libstopwatch")
INCLUDE_DIRECTORIES ("${LIB_STOPWATCH_ROOT}")

#ADD_DEFINITIONS ("-DLIB_STATE_MACHINE_DEFAULT_EVENT_TYPE=BinaryEvent")

# Resonator used in this project
ADD_DEFINITIONS (-DHSE_VALUE=8000000)

# Override linker script from libmicro cmake
# SET (LINKER_SCRIPT "../src/STM32F407VG_FLASH.ld")

#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=rdimon.specs -Wl,--gc-sections" CACHE INTERNAL "")
#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -specs=nosys.specs -Wl,--gc-sections" CACHE INTERNAL "")
#SET (CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -Wl,--gc-sections" CACHE INTERNAL "")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj" CACHE INTERNAL "")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps=obj" CACHE INTERNAL "")

# +-----------------+
# | My code         |
# +-----------------+
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf
    "src/main.cc"

    "${LIB_STOPWATCH_ROOT}/Adc.cc"
    "${LIB_STOPWATCH_ROOT}/AdcChannel.cc"
    "${LIB_STOPWATCH_ROOT}/AdcChannel.h"
    "${LIB_STOPWATCH_ROOT}/Adc.h"
    "${LIB_STOPWATCH_ROOT}/Button.cc"
    "${LIB_STOPWATCH_ROOT}/Button.h"
    "${LIB_STOPWATCH_ROOT}/Buzzer.cc"
    "${LIB_STOPWATCH_ROOT}/Buzzer.h"
    "${LIB_STOPWATCH_ROOT}/FastStateMachine.cc"
    "${LIB_STOPWATCH_ROOT}/FastStateMachine.h"
    "${LIB_STOPWATCH_ROOT}/History.cc"
    "${LIB_STOPWATCH_ROOT}/History.h"
    "${LIB_STOPWATCH_ROOT}/IDisplay.h"
    "${LIB_STOPWATCH_ROOT}/ILcdDataLink.h"
    "${LIB_STOPWATCH_ROOT}/ILcdDdriver.h"
    "${LIB_STOPWATCH_ROOT}/InfraRedBeam.cc"
    "${LIB_STOPWATCH_ROOT}/InfraRedBeam.h"
    "${LIB_STOPWATCH_ROOT}/StopWatch.cc"
    "${LIB_STOPWATCH_ROOT}/StopWatch.h"
    "${LIB_STOPWATCH_ROOT}/Led7SegmentDisplay.cc"
    "${LIB_STOPWATCH_ROOT}/Led7SegmentDisplay.h"
    "${LIB_STOPWATCH_ROOT}/CanProtocol.cc"
    "${LIB_STOPWATCH_ROOT}/CanProtocol.h"
    "${LIB_STOPWATCH_ROOT}/DisplayMenu.cc"
    "${LIB_STOPWATCH_ROOT}/DisplayMenu.h"


    #"${LIB_STOPWATCH_ROOT}/usbd_cdc_interface.c"
    #"${LIB_STOPWATCH_ROOT}/usbd_cdc_interface.h"
    #"${LIB_STOPWATCH_ROOT}/usbd_conf.c"
    #"${LIB_STOPWATCH_ROOT}/usbd_conf.h"
    #"${LIB_STOPWATCH_ROOT}/usbd_desc.c"
    #"${LIB_STOPWATCH_ROOT}/usbd_desc.h"
    #"${LIB_STOPWATCH_ROOT}/utils.cc"
    #"${LIB_STOPWATCH_ROOT}/utils.h"

    "${LIB_MICRO_ROOT}/src/Blinker.h"
    "${LIB_MICRO_ROOT}/src/ErrorHandler.h"
    "${LIB_MICRO_ROOT}/src/ErrorHandler.c"
    "${LIB_MICRO_ROOT}/src/Hal.h"
    "${LIB_MICRO_ROOT}/src/HalStm32F0.h"
    "${LIB_MICRO_ROOT}/src/TimerF.cc"
    "${LIB_MICRO_ROOT}/src/Timer.h"
    "${LIB_MICRO_ROOT}/src/HardwareTimer.cc"
    "${LIB_MICRO_ROOT}/src/HardwareTimer.h"
    "${LIB_MICRO_ROOT}/src/Debug.cc"
    "${LIB_MICRO_ROOT}/src/Debug.h"
    "${LIB_MICRO_ROOT}/src/debug.h"
    "${LIB_MICRO_ROOT}/src/Gpio.h"
    "${LIB_MICRO_ROOT}/src/GpioF.h"
    "${LIB_MICRO_ROOT}/src/GpioF.cc"
    "${LIB_MICRO_ROOT}/src/GpioF0.cc"
    "${LIB_MICRO_ROOT}/src/Usart.h"
    "${LIB_MICRO_ROOT}/src/UsartF.cc"
    "${LIB_MICRO_ROOT}/src/UsartF.h"
    "${LIB_MICRO_ROOT}/src/UsartF0.cc"
    "${LIB_MICRO_ROOT}/src/Spi.h"
    "${LIB_MICRO_ROOT}/src/SpiF.cc"
    "${LIB_MICRO_ROOT}/src/SpiF.h"
    "${LIB_MICRO_ROOT}/src/SpiF0.cc"
    "${LIB_MICRO_ROOT}/src/Can.h"
    "${LIB_MICRO_ROOT}/src/CanF.cc"
    "${LIB_MICRO_ROOT}/src/CanFrame.h"
    "${LIB_MICRO_ROOT}/src/CanFrame.cc"
)

# +--------------+
# | STM Cube     |
# +--------------+
# Workaround : rozdzielenie kodu C++ i C powoduje, że QTCreator dobrze parsuje nagłówki C++. Bez tego podziału QTCreator się gubi.
add_library ("stm" STATIC
#        "src/startup.c"
#        "src/syscalls.c"
        "${STARTUP_CODE}"
        "src/stm32f0xx_it.c"
        "src/stm32f0xx_hal_conf.h"
        "src/system_stm32f0xx.c"

        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_can.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cec.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_comp.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_crc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_crc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dac.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dac_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2s.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_irda.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_iwdg.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smartcard.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smartcard_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smbus.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_spi.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_spi_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tsc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart_ex.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_usart.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_wwdg.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_comp.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_crc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_crs.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dac.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rtc.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_spi.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.c"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.c"

        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_adc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_adc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_can.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_cec.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_comp.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_cortex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_crc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_crc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_dac_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_dac.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_def.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_dma_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_dma.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_flash_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_flash.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_gpio_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_gpio.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_i2c_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_i2c.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_i2s.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_irda_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_irda.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_iwdg.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_pcd_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_pcd.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_pwr_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_pwr.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_rcc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_rcc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_rtc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_rtc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_smartcard_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_smartcard.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_smbus.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_spi_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_spi.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_tim_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_tim.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_tsc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_uart_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_uart.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_usart_ex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_usart.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_hal_wwdg.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_adc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_bus.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_comp.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_cortex.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_crc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_crs.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_dac.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_dma.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_exti.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_gpio.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_i2c.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_iwdg.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_pwr.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_rcc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_rtc.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_spi.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_system.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_tim.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_usart.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_utils.h"
        "${CUBE_ROOT}/Drivers/STM32F0xx_HAL_Driver/Inc/stm32f0xx_ll_wwdg.h"
)

TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME}.elf -Wl,--whole-archive stm -Wl,--no-whole-archive)

FIND_PROGRAM (OPENOCD openocd)
ADD_CUSTOM_TARGET("upload" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg -c 'program ${CMAKE_PROJECT_NAME}.elf verify reset exit')
ADD_CUSTOM_TARGET("debug" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg)

#ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.bin ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)

