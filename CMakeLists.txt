cmake_minimum_required(VERSION 3.13)
# set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")

include(pico_sdk_import.cmake)
#name of the project and last 3 arguments are for using cpp
project(NAND_FLASH C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(main main.cpp )

# pull in common dependencies
target_link_libraries(main pico_stdlib pico_multicore hardware_adc )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)
