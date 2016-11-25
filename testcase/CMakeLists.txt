# Customize linker commands
set(CMAKE_LINKER "ld")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(
        CMAKE_C_LINK_EXECUTABLE
        "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> \
        <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

set(UCLIB_PATH ${PROJECT_SOURCE_DIR}/lib-common/uclibc)
include_directories(${PROJECT_SOURCE_DIR}/lib-common)
include_directories(${UCLIB_PATH}/include)

# Find all sources
file(GLOB_RECURSE TESTCASE_SOURCE_FILES "*.c")

# Add the special "mov.S"
set(MOV_ASM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/mov.S)
set_property(SOURCE ${MOV_ASM_SRC} PROPERTY LANGUAGE C)
list(APPEND TESTCASE_SOURCE_FILES ${MOV_ASM_SRC})

# Create executables
foreach (CASE_SRC ${TESTCASE_SOURCE_FILES})
    # Set variables
    get_filename_component(EXE_NAME ${CASE_SRC} NAME_WE)
    get_filename_component(SRC_EXT ${CASE_SRC} EXT)
    set(RUN_CASE ${EXE_NAME}_run)

    add_executable(${EXE_NAME} ${CASE_SRC})
    target_compile_options(
            ${EXE_NAME} PRIVATE
            -m32 -fno-builtin
            -fno-stack-protector -Wno-format
    )
    if ("${SRC_EXT}" STREQUAL ".S")
        # For assembly sources, do not link any lib
        set(ADD_LINK_FLAGS "-m elf_i386 -e _start -Ttext=0x00800000")
    else ()
        # For special C sources
        if ("${EXE_NAME}" STREQUAL "integral" OR
                "${EXE_NAME}" STREQUAL "quadratic-eq")
            target_link_libraries(${EXE_NAME} float)
            target_compile_options(${EXE_NAME} PRIVATE -O2)
        endif ()

        # For normal C sources
        target_link_libraries(${EXE_NAME} uclibc float)
        set(ADD_LINK_FLAGS "-m elf_i386 -e main -Ttext-segment=0x00800000")
    endif ()
    set_property(
            TARGET ${EXE_NAME}
            APPEND PROPERTY LINK_FLAGS
            "${ADD_LINK_FLAGS}"
    )

    set(EXE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME})

    add_custom_target(
            ${RUN_CASE}
            COMMAND cd ${NEMU_PATH} && ${NEMU_BIN} ${EXE_FILE}
            DEPENDS entry nemu ${EXE_NAME}
    )
    list(APPEND TEST_CASES ${EXE_NAME})
    list(APPEND TEST_FILES ${EXE_FILE})
endforeach ()

# Add a custom target to build all cases
add_custom_target(testcases DEPENDS ${TEST_CASES})

# Add a target to run all test cases
STRING(REPLACE ";" " " TEST_FILES "${TEST_FILES}")
add_custom_target(
        testall DEPENDS nemu testcases
        COMMAND cd ${NEMU_PATH} && export nemu=${NEMU_BIN} && bash ${PROJECT_SOURCE_DIR}/test.sh ${TEST_FILES}
)

#
# print-FLOAT for Linux
#

add_executable(print-FLOAT-linux ${CMAKE_CURRENT_SOURCE_DIR}/src/print-FLOAT.c)
target_compile_options(
        print-FLOAT-linux PRIVATE
        -m32 -fno-builtin
        -fno-stack-protector -Wno-format
)
target_compile_definitions(print-FLOAT-linux PRIVATE -DLINUX_RT)
target_link_libraries(
        print-FLOAT-linux
        ${UCLIB_PATH}/lib/crt1.o
        ${UCLIB_PATH}/lib/crti.o
        float
        uclibc
        ${UCLIB_PATH}/lib/crtn.o
)
set_property(
        TARGET print-FLOAT-linux
        APPEND PROPERTY LINK_FLAGS
        "-m elf_i386"
)
