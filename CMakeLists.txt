cmake_minimum_required(VERSION 3.10)
project(win32)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(win32
        gpio_protocol.cpp
        gpio_protocol.h
        mplc_gpio.cpp
        mplc_gpio.h
        stdafx.cpp
        stdafx.h
        types.h)
