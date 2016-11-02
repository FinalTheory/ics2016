set(NEMU_SOURCE_FILES
        src/cpu/decode/decode.c
        src/cpu/decode/modrm.c
        src/cpu/exec/arith/dec.c
        src/cpu/exec/arith/div.c
        src/cpu/exec/arith/idiv.c
        src/cpu/exec/arith/imul.c
        src/cpu/exec/arith/inc.c
        src/cpu/exec/arith/mul.c
        src/cpu/exec/arith/neg.c
        src/cpu/exec/data-mov/mov.c
        src/cpu/exec/data-mov/xchg.c
        src/cpu/exec/logic/and.c
        src/cpu/exec/logic/not.c
        src/cpu/exec/logic/or.c
        src/cpu/exec/logic/sar.c
        src/cpu/exec/logic/shl.c
        src/cpu/exec/logic/shr.c
        src/cpu/exec/logic/shrd.c
        src/cpu/exec/logic/xor.c
        src/cpu/exec/misc/misc.c
        src/cpu/exec/prefix/prefix.c
        src/cpu/exec/special/special.c
        src/cpu/exec/string/rep.c
        src/cpu/exec/exec.c
        src/cpu/reg.c
        src/device/io/mmio.c
        src/device/io/port-io.c
        src/device/device.c
        src/device/i8259.c
        src/device/ide.c
        src/device/keyboard.c
        src/device/sdl.c
        src/device/serial.c
        src/device/timer.c
        src/device/vga-palette.c
        src/device/vga.c
        src/lib/logo.c
        src/memory/dram.c
        src/memory/memory.c
        src/monitor/debug/elf.c
        src/monitor/debug/expr.c
        src/monitor/debug/ui.c
        src/monitor/debug/watchpoint.c
        src/monitor/cpu-exec.c
        src/monitor/monitor.c
        src/main.c)


set(FLEX_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/eval-flex.c)
set(FLEX_HEADER ${CMAKE_CURRENT_BINARY_DIR}/eval-flex.h)
find_package(FLEX)
FLEX_TARGET(
        Lexer src/monitor/debug/eval.l
        ${FLEX_SOURCE}
        COMPILE_FLAGS "-R --bison-bridge --header-file=${FLEX_HEADER}")


set(BISON_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/eval-parse.c)
set(BISON_HEADER ${CMAKE_CURRENT_BINARY_DIR}/eval-parse.h)
find_package(BISON)
BISON_TARGET(
        Parser src/monitor/debug/eval.y
        ${BISON_SOURCE}
        COMPILE_FLAGS "-d -v -y --debug --defines=${BISON_HEADER}")


include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

add_executable(nemu
        ${NEMU_SOURCE_FILES}
        ${FLEX_Lexer_OUTPUTS}
        ${BISON_Parser_OUTPUTS})

target_link_libraries(nemu readline l)
