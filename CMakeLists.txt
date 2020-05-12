CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE OFF)
SET (CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

PROJECT (receiver)
ENABLE_LANGUAGE (ASM-ATT)

INCLUDE_DIRECTORIES("src/")
INCLUDE_DIRECTORIES("deps/etl/include/")
INCLUDE_DIRECTORIES("deps/GSL/include/")
INCLUDE_DIRECTORIES("deps/STM32F0xx_HAL_Driver/Inc/")
INCLUDE_DIRECTORIES("deps/CMSIS/Device/ST/STM32F0xx/Include/")
INCLUDE_DIRECTORIES("deps/CMSIS/Device/ST/STM32F0xx/Include/Legacy")
INCLUDE_DIRECTORIES("deps/CMSIS/Include/")
INCLUDE_DIRECTORIES("deps/STM32_USB_Device_Library/Core/Inc")
INCLUDE_DIRECTORIES("deps/STM32_USB_Device_Library/Class/CDC/Inc")

SET (LIB_MICRO_ROOT "deps/libmicro")
INCLUDE_DIRECTORIES ("${LIB_MICRO_ROOT}/src")

# ADD_DEFINITIONS (-DWITH_SOUND=1)
# ADD_DEFINITIONS (-DWITH_DISPLAY=1)
ADD_DEFINITIONS (-DWITH_USB=1)


ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf
    "src/main.cc"
    "src/Adc.cc"
    "src/AdcChannel.cc"
    "src/Button.cc"
    "src/Buzzer.cc"
    "src/FastStateMachine.cc"
    "src/History.cc"
    "src/InfraRedBeam.cc"
    "src/StopWatch.cc"
    "src/Led7SegmentDisplay.cc"
    "src/CanProtocol.cc"
    "src/DisplayMenu.cc"
    "src/Rtc.cc"

    "src/usbd_cdc_interface.c"
    "src/usbd_conf.c"
    "src/usbd_desc.c"
    
    "${LIB_MICRO_ROOT}/src/ErrorHandler.c"
    "${LIB_MICRO_ROOT}/src/TimerF.cc"
    "${LIB_MICRO_ROOT}/src/HardwareTimer.cc"
    "${LIB_MICRO_ROOT}/src/Debug.cc"
    "${LIB_MICRO_ROOT}/src/GpioF.cc"
    "${LIB_MICRO_ROOT}/src/GpioF0.cc"
    "${LIB_MICRO_ROOT}/src/UsartF.cc"
    "${LIB_MICRO_ROOT}/src/UsartF0.cc"
    "${LIB_MICRO_ROOT}/src/SpiF.cc"
    "${LIB_MICRO_ROOT}/src/SpiF0.cc"
    "${LIB_MICRO_ROOT}/src/CanF.cc"
    "${LIB_MICRO_ROOT}/src/CanFrame.cc"

    "${STARTUP_CODE}"
    "src/stm32f0xx_it.c"
    "src/system_stm32f0xx.c"

    "deps/STM32_USB_Device_Library/Core/Src/usbd_core.c"
    "deps/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c"
    "deps/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c"
    "deps/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c"

    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c"
    "deps/STM32F0xx_HAL_Driver/Src/Legacy/stm32f0xx_hal_can.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cec.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_comp.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_crc.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_crc_ex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dac.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dac_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2s.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_irda.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_iwdg.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc_ex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smartcard.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smartcard_ex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_smbus.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_spi.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_spi_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tsc.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart_ex.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_usart.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_wwdg.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_comp.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_crc.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_crs.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dac.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rtc.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_spi.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.c"
    "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usb.c"
    # "deps/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.c"
)

FIND_PROGRAM (OPENOCD openocd)
ADD_CUSTOM_TARGET("upload" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg -c 'program ${CMAKE_PROJECT_NAME}.elf verify reset exit')
ADD_CUSTOM_TARGET("debug" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32f0x.cfg)
ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.bin ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)

FIND_PROGRAM (ELF2DFUSE elf2dfuse)
ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.dfu ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${ELF2DFUSE} ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.dfu)
