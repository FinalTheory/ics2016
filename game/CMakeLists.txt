# Customize linker commands
set(CMAKE_LINKER "ld")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(
        CMAKE_C_LINK_EXECUTABLE
        "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> \
        <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

include_directories(include)
include_directories(src/typing/include)
include_directories(${CMAKE_SOURCE_DIR}/lib-common)
include_directories(${CMAKE_SOURCE_DIR}/lib-common/uclibc/include)

add_compile_options(
        -m32 -O2 -fno-builtin
        -fno-omit-frame-pointer
        -fno-stack-protector
        -Wno-format
)

# Glob sources
file(GLOB_RECURSE COMMON_SOURCE_FILES "src/common/*.c")
file(GLOB_RECURSE TYPING_SOURCE_FILES "src/typing/*.c")
file(GLOB_RECURSE PAL_SOURCE_FILES "src/nemu-pal/*.c")

# Add kernel binary
add_executable(typing ${COMMON_SOURCE_FILES} ${TYPING_SOURCE_FILES})

set_property(
        TARGET typing
        APPEND PROPERTY LINK_FLAGS
        "-m elf_i386"
)

target_link_libraries(
        typing
        ${UCLIB_PATH}/lib/crt1.o
        ${UCLIB_PATH}/lib/crti.o
        float
        uclibc
        ${UCLIB_PATH}/lib/crtn.o
)
