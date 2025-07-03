# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(synth_audio C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable( synth_audio
  main.c
  ./include/ssd1306_i2c.c
)

pico_set_program_name(synth_audio "synth_audio")
pico_set_program_version(synth_audio "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(synth_audio 0)
pico_enable_stdio_usb(synth_audio 1)

# Add the standard library to the build
target_link_libraries(synth_audio
        # Biblioteca padrão do Pico SDK, que fornece funções básicas para o RP2040 (GPIO, temporizadores, UART, etc.).
        pico_stdlib
        )

# Add the standard include files to the build
target_include_directories(synth_audio PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any user requested libraries
target_link_libraries(synth_audio 
        hardware_adc
        hardware_dma
        hardware_pwm
        hardware_i2c
)

pico_add_extra_outputs(synth_audio)

