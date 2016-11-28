file(GLOB_RECURSE NEMU_SOURCE_FILES "*.c")

set(FLEX_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/eval-flex.c)
set(FLEX_HEADER ${CMAKE_CURRENT_BINARY_DIR}/eval-flex.h)
find_package(FLEX)
FLEX_TARGET(
        Lexer src/monitor/debug/eval.l
        ${FLEX_SOURCE}
        COMPILE_FLAGS "-R --bison-bridge --header-file=${FLEX_HEADER}"
)


set(BISON_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/eval-parse.c)
set(BISON_HEADER ${CMAKE_CURRENT_BINARY_DIR}/eval-parse.h)
find_package(BISON)
BISON_TARGET(
        Parser src/monitor/debug/eval.y
        ${BISON_SOURCE}
        COMPILE_FLAGS "-d -v -y --debug --defines=${BISON_HEADER}"
)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

add_library(eval STATIC ${FLEX_Lexer_OUTPUTS} ${BISON_Parser_OUTPUTS})

add_executable(nemu ${NEMU_SOURCE_FILES})

set(NEMU_PATH ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(NEMU_BIN ${CMAKE_CURRENT_BINARY_DIR}/nemu PARENT_SCOPE)

if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
    target_compile_options(
            eval PRIVATE
            -Wno-unused-function
            -Wno-implicit-function-declaration
            -Wno-unneeded-internal-declaration
    )
    target_compile_options(
            nemu PRIVATE
            -Wno-unused-variable
            -Wno-tautological-constant-out-of-range-compare
    )
else()
    target_compile_options(
            eval PRIVATE
            -Wno-error
    )
    target_compile_options(
            nemu PRIVATE
            -Wno-unused-variable
            -Wno-unused-but-set-variable
    )
endif()

target_link_libraries(nemu eval readline l)
