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
include_directories(${CMAKE_SOURCE_DIR}/nemu/include)

# Create linker script
set(LD_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/user.ld)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/user.ld LD_SCRIPT_DATA)
string(
        REPLACE "obj/testcase/start.o"
        "${CMAKE_CURRENT_BINARY_DIR}/start.S.o"
        LD_SCRIPT_DATA "${LD_SCRIPT_DATA}"
)
file(WRITE ${LD_SCRIPT} "${LD_SCRIPT_DATA}")

# Glob sources
file(GLOB_RECURSE KERNEL_SOURCE_FILES "*.c")

# Add kernel binary
add_executable(kernel ${KERNEL_SOURCE_FILES} src/irq/do_irq.S)

# Add start.S as entry point
add_library(START_LIB STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/start.S)
add_custom_target(
        extract_start
        COMMAND cd ${CMAKE_CURRENT_BINARY_DIR} && ar x $<TARGET_FILE:START_LIB>
        DEPENDS START_LIB
)

add_dependencies(kernel extract_start)

target_compile_options(
        kernel PRIVATE
        -m32 -O2 -fno-builtin
        -fno-omit-frame-pointer
        -fno-stack-protector
)

set_property(
        TARGET kernel
        APPEND PROPERTY LINK_FLAGS
        "-m elf_i386 -T ${LD_SCRIPT}"
)

set(MM_MALLOC_OBJ ${CMAKE_CURRENT_SOURCE_DIR}/src/memory/mm_malloc.o)
set_source_files_properties(
        ${MM_MALLOC_OBJ}
        PROPERTIES
        EXTERNAL_OBJECT true
        GENERATED true
)

target_link_libraries(kernel uclibc ${MM_MALLOC_OBJ})

add_custom_target(
        entry
        COMMAND objcopy -S -O binary ${CMAKE_CURRENT_BINARY_DIR}/kernel ${NEMU_PATH}/entry
        DEPENDS kernel
)
