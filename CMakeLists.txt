# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Set the board type (pico or pico_w)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Project name and version
project(systemWorking C CXX ASM)

# Add executables for both projects
add_executable(systemWorking systemWorking.c)

# Set program names and versions
pico_set_program_name(systemWorking "systemWorking")
pico_set_program_version(systemWorking "0.1")

# Generate PIO header for neopixel_pio
pico_generate_pio_header(systemWorking ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Enable/disable output over UART/USB
pico_enable_stdio_uart(systemWorking 0)
pico_enable_stdio_usb(systemWorking 1)

# Add the standard library to the build
target_link_libraries(systemWorking
        pico_stdlib
        hardware_pwm
        hardware_pio)

# Add the standard include files to the build
target_include_directories(systemWorking PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}  # Adicione este diretório para incluir o arquivo gerado
)

# Add any user requested libraries (if needed)
# target_link_libraries(systemWorking ...)

# Add extra outputs (UF2, HEX, etc.)
pico_add_extra_outputs(systemWorking)