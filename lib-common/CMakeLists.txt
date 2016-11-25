include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add float lib
add_library(
        float STATIC
        ${PROJECT_SOURCE_DIR}/lib-common/FLOAT/FLOAT.c
        ${PROJECT_SOURCE_DIR}/lib-common/FLOAT/FLOAT_vfprintf.c
)
target_compile_options(
        float PRIVATE
        -m32 -fno-builtin
        -fno-stack-protector -Wno-unused
)

# For Linux platform only
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Add "uclibc" as imported library
    add_library(uclibc STATIC IMPORTED GLOBAL)
    set_property(
            TARGET uclibc PROPERTY IMPORTED_LOCATION
            ${CMAKE_CURRENT_SOURCE_DIR}/uclibc/lib/libc.a
    )

    add_executable(float_test ${CMAKE_CURRENT_SOURCE_DIR}/FLOAT/FLOAT_test.c)
    target_link_libraries(float_test float)

    target_compile_options(
            float_test PRIVATE
            -m32 -fno-builtin
            -fno-stack-protector
    )
endif ()
