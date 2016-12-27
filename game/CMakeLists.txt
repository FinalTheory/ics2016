# Customize linker commands
set(CMAKE_LINKER "ld")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(
        CMAKE_C_LINK_EXECUTABLE
        "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> \
        <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/lib-common)
include_directories(${CMAKE_SOURCE_DIR}/lib-common/uclibc/include)

add_compile_options(
        -m32 -O2 -fno-builtin
        -fno-omit-frame-pointer
        -fno-stack-protector
        -Wno-format
        -Wno-unused-variable
)

# Glob sources
file(GLOB_RECURSE COMMON_SOURCE_FILES "src/common/*.c")
file(GLOB_RECURSE TYPING_SOURCE_FILES "src/typing/*.c")
file(GLOB_RECURSE PAL_SOURCE_FILES "src/nemu-pal/*.c")

link_libraries(
        ${UCLIB_PATH}/lib/crt1.o
        ${UCLIB_PATH}/lib/crti.o
        float
        uclibc
        ${UCLIB_PATH}/lib/crtn.o
)

# Add kernel binary
add_executable(typing ${COMMON_SOURCE_FILES} ${TYPING_SOURCE_FILES})
add_executable(pal ${COMMON_SOURCE_FILES} ${PAL_SOURCE_FILES})

target_include_directories(typing PRIVATE src/typing/include)
target_include_directories(pal PRIVATE src/nemu-pal/include)

set_property(
        TARGET typing pal
        APPEND PROPERTY LINK_FLAGS
        "-m elf_i386"
)
