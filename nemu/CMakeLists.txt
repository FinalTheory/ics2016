file(GLOB_RECURSE NEMU_SOURCE_FILES "*.c")

find_package(SDL REQUIRED)

include_directories(
        include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib-common
        ${SDL_INCLUDE_DIR}
)

add_executable(nemu ${NEMU_SOURCE_FILES})

set(NEMU_PATH ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(NEMU_BIN ${CMAKE_CURRENT_BINARY_DIR}/nemu PARENT_SCOPE)

if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
    target_compile_options(
            nemu PRIVATE
            -Wno-unused-variable
            -Wno-tautological-constant-out-of-range-compare
    )
else()
    target_compile_options(
            nemu PRIVATE
            -Wno-unused-variable
            -Wno-unused-but-set-variable
            -Wno-format
            -Wno-strict-aliasing
    )
endif()

target_link_libraries(nemu readline ${SDL_LIBRARY})
