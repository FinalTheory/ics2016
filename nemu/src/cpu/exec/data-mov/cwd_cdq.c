#include "cpu/exec/helper.h"

make_helper(cbw_w) {
    reg_w(R_AX) = (int8_t)reg_b(R_AL);
    print_asm("cbtw");
    return 1;
}

make_helper(cbw_l) {
    reg_l(R_EAX) = (int16_t)reg_w(R_AX);
    print_asm("cwtl");
    return 1;
}

make_helper(cwd_w) {
    uint32_t val = (int16_t)reg_w(R_AX);
    uint32_t mask = ((uint32_t)~0) >> 16;
    reg_w(R_AX) = (uint16_t)(val & mask);
    reg_w(R_DX) = (uint16_t)((val & ~mask) >> 16);
    print_asm("cwtd");
    return 1;
}


make_helper(cwd_l) {
    uint64_t val = (int32_t)reg_l(R_EAX);
    uint64_t mask = ((uint64_t)~0) >> 32;
    reg_l(R_EAX) = (uint32_t)(val & mask);
    reg_l(R_EDX) = (uint32_t)((val & ~mask) >> 32);
    print_asm("cltd");
    return 1;
}

make_helper_v(cwd);

make_helper_v(cbw);
