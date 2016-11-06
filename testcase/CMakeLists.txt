# Customize linker commands
set(CMAKE_LINKER "ld")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(
        CMAKE_C_LINK_EXECUTABLE
        "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> \
        <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

include_directories(../lib-common)
include_directories(../lib-common/newlib/include)

# Find all sources
file(GLOB_RECURSE TESTCASE_SOURCE_FILES "*.c")

# Add the special "mov.S"
set(MOV_ASM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/mov.S)
set_property(SOURCE ${MOV_ASM_SRC} PROPERTY LANGUAGE C)
list(APPEND TESTCASE_SOURCE_FILES ${MOV_ASM_SRC})

# Add start.S as entry point
set(START_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/start.S)

# Add float lib
add_library(float STATIC ../lib-common/FLOAT.c)
target_compile_options(float PRIVATE -fno-builtin)

# Add "newlib" as imported library
set(NEWLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib-common/newlib/libc.a)
add_library(newlib STATIC IMPORTED)
set_property(TARGET newlib PROPERTY IMPORTED_LOCATION ${NEWLIB_PATH})

# Create executables
foreach (CASE_SRC ${TESTCASE_SOURCE_FILES})
    get_filename_component(EXE_NAME ${CASE_SRC} NAME_WE)
    get_filename_component(SRC_EXT ${CASE_SRC} EXT)
    set(ENTRY_NAME ${EXE_NAME}_entry)
    set(RUN_CASE ${EXE_NAME}_run)
    if (${SRC_EXT} MATCHES ".c")
        add_executable(${EXE_NAME} ${START_SRC} ${CASE_SRC})
    else ()
        add_executable(${EXE_NAME} ${CASE_SRC})
    endif ()
    target_compile_options(
            ${EXE_NAME} PRIVATE
            -m32 -fno-builtin
            -fno-stack-protector
    )
    set_property(
            TARGET ${EXE_NAME}
            APPEND PROPERTY LINK_FLAGS
            "-m elf_i386 -e start -Ttext=0x00100000"
    )
    target_link_libraries(${EXE_NAME} float newlib)
    set(EXE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME})
    set(ENTRY_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ENTRY_NAME})
    add_custom_target(
            ${ENTRY_NAME}
            COMMAND objcopy -S -O binary ${EXE_FILE} ${ENTRY_FILE}
            DEPENDS ${EXE_NAME}
    )
    add_custom_target(
            ${RUN_CASE}
            COMMAND ${NEMU_BIN} ${EXE_FILE}
            DEPENDS ${EXE_NAME} nemu
    )
    list(APPEND TEST_CASES ${ENTRY_NAME})
endforeach ()

# Add a custom target to build all cases
add_custom_target(testcases DEPENDS ${TEST_CASES})
