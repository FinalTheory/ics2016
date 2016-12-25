#include <nemu.h>
#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "movs-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "movs-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "movs-template.h"
#undef DATA_BYTE


make_helper_v(movs)

make_helper(cld) {
    cpu.eflags.DF = 0;
    print_asm("cld");
    return 1;
}

make_helper(std) {
    cpu.eflags.DF = 1;
    print_asm("std");
    return 1;
}
