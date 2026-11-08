# hal/CMakeLists.txt
# Build STM32F1xx HAL as a STATIC library

set(HAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stm32f1xx-hal/")
set(DEVICES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Devices/")
set(CMSIS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/")

set(HAL_SOURCES
    ${HAL_DIR}/Src/stm32f1xx_hal.c
    ${HAL_DIR}/Src/stm32f1xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32f1xx_hal_gpio_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_uart.c
    ${HAL_DIR}/Src/stm32f1xx_hal_exti.c
    ${HAL_DIR}/Src/stm32f1xx_hal_dma.c
    ${HAL_DIR}/Src/stm32f1xx_hal_eth.c
    ${HAL_DIR}/Src/stm32f1xx_hal_can.c
    ${HAL_DIR}/Src/Legacy/stm32f1xx_hal_can.c
    ${HAL_DIR}/Src/stm32f1xx_hal_cec.c
    ${HAL_DIR}/Src/stm32f1xx_hal_adc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_adc_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_crc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_dac.c
    ${HAL_DIR}/Src/stm32f1xx_hal_dac_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_sram.c
    ${HAL_DIR}/Src/stm32f1xx_hal_nor.c
    ${HAL_DIR}/Src/stm32f1xx_hal_i2c.c
    ${HAL_DIR}/Src/stm32f1xx_hal_i2s.c
    ${HAL_DIR}/Src/stm32f1xx_hal_iwdg.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rtc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rtc_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pccard.c
    ${HAL_DIR}/Src/stm32f1xx_hal_sd.c
    ${HAL_DIR}/Src/stm32f1xx_hal_nand.c
    ${HAL_DIR}/Src/stm32f1xx_hal_spi.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_usart.c
    ${HAL_DIR}/Src/stm32f1xx_hal_irda.c
    ${HAL_DIR}/Src/stm32f1xx_hal_smartcard.c
    ${HAL_DIR}/Src/stm32f1xx_hal_wwdg.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pcd.c
    ${HAL_DIR}/Src/stm32f1xx_hal_hcd.c
    ${HAL_DIR}/Src/stm32f1xx_hal_mmc.c
)

add_library(stm32f1xx_hal STATIC ${HAL_SOURCES})

target_compile_options(stm32f1xx_hal PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -w
)

target_include_directories(stm32f1xx_hal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HAL_DIR}/Inc
    ${DEVICES_DIR}/Include
    ${CMSIS_DIR}/CM3
)

target_compile_definitions(stm32f1xx_hal PUBLIC
    STM32F10X_MD_VL
    USE_HAL_DRIVER
    STM32F103xB
)

target_compile_options(stm32f1xx_hal PRIVATE -w)