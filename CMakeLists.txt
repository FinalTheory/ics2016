cmake_minimum_required(VERSION 3.0)
project(ics2015 C ASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -Wall -Werror")

# Only emulator could be built on macOS
add_subdirectory(nemu)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_subdirectory(testcase)
endif ()
