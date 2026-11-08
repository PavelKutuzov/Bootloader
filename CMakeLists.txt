cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------
# Project
# ---------------------------------------------------------------------------
project(Bootloader C ASM)

# ---------------------------------------------------------------------------
# Build directory
# ---------------------------------------------------------------------------
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/artifacts/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}")

# ---------------------------------------------------------------------------
# Options / flags
# ---------------------------------------------------------------------------
set(DEBUG 1 CACHE BOOL "Debug build")

set(CPU_FLAGS -mcpu=cortex-m3 -mthumb)

set(C_INCLUDES
    codebase/main_fw/application
    codebase/main_fw/hardware/CMSIS/CM3
    codebase/main_fw/middleware
    codebase/main_fw/HAL/Devices/Include
    codebase/main_fw/middleware/ring_buffer
    codebase/main_fw/middleware/uart_proto
)

set(C_SOURCES
    codebase/main_fw/application/main.c
    codebase/main_fw/application/app.c
    codebase/main_fw/HAL/Devices/Source/Templates/system_stm32f1xx.c
    codebase/main_fw/middleware/ring_buffer/ring_buffer.c
    codebase/main_fw/middleware/uart_proto/uart_proto.c
)

set(ASM_SOURCES
    codebase/main_fw/hardware/system/startup_stm32f10x_md_vl.s
)

set(LDSCRIPT "${CMAKE_SOURCE_DIR}/codebase/main_fw/hardware/system/stm32f103_boot.ld")

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}.elf ${C_SOURCES} ${ASM_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}.elf PRIVATE ${C_INCLUDES})

# Defines
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE ${C_DEFS})

# C compile flags
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${CPU_FLAGS}
    -std=c99
    -Og
    -Wall
    -fdata-sections
    -ffunction-sections
    -g3
    -gdwarf
)

# ASM flags (gcc -x assembler-with-cpp)
# ASM flags (gcc -x assembler-with-cpp)
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    $<$<COMPILE_LANGUAGE:ASM>:${CPU_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>
    $<$<COMPILE_LANGUAGE:ASM>:-Wall>
    $<$<COMPILE_LANGUAGE:ASM>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:ASM>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:ASM>:-Og>
)

# Link flags
target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${CPU_FLAGS}
    -specs=nano.specs
    -T${LDSCRIPT}
    -Wl,-Map=${BUILD_DIR}/${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Add HAL library
add_subdirectory(codebase/main_fw/HAL)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    stm32f1xx_hal
    c m nosys
)

# ---------------------------------------------------------------------------
# Post-build: .bin and size
# ---------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${BUILD_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${PROJECT_NAME}.elf> ${BUILD_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .hex / .bin and printing size"
)

# ---------------------------------------------------------------------------
# Custom clean: remove the whole build directory
# ---------------------------------------------------------------------------
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${BUILD_DIR}"
    COMMENT "Removing ${BUILD_DIR}"
    USES_TERMINAL
)