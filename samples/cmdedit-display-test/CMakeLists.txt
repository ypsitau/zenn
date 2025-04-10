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
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(cmdedit-display-test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(cmdedit-display-test cmdedit-display-test.cpp )

pico_set_program_name(cmdedit-display-test "cmdedit-display-test")
pico_set_program_version(cmdedit-display-test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(cmdedit-display-test 1)
pico_enable_stdio_usb(cmdedit-display-test 0)

# Add the standard library to the build
target_link_libraries(cmdedit-display-test
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(cmdedit-display-test PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(cmdedit-display-test)

target_link_libraries(cmdedit-display-test jxglib_ST7789)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
