#include "cpu/exec/helper.h"

#define make_lods_helper(name, type, reg) make_helper(name) { \
    reg = (type)swaddr_read((swaddr_t)reg_l(R_ESI), sizeof(type)); \
    print_asm(__func__); \
    if (cpu.eflags.DF) { \
        reg_l(R_ESI) -= sizeof(type); \
    } else { \
        reg_l(R_ESI) += sizeof(type); \
    } \
    return 1; \
}

make_lods_helper(lods_b, uint8_t, reg_b(R_AL))

make_lods_helper(lods_w, uint16_t, reg_w(R_AX))

make_lods_helper(lods_l, uint32_t, reg_l(R_EAX))

make_helper_v(lods)
