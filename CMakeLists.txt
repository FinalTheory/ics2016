cmake_minimum_required(VERSION 3.0)
project(ics2016 C ASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -Wall -Werror")

set(UCLIB_PATH ${PROJECT_SOURCE_DIR}/lib-common/uclibc)

# Useful macro to remove compiler flag
macro(remove_c_flag flag)
    string(REPLACE "${flag}" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endmacro()

# Only emulator could be built on macOS
add_subdirectory(nemu)

add_subdirectory(lib-common)

add_subdirectory(kernel)

add_subdirectory(game)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_subdirectory(testcase)
endif ()
