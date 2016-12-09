#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat3(instr, _, SUFFIX)) {
    DATA_TYPE x = (DATA_TYPE)reg_l(R_EAX);
    DATA_TYPE y = (DATA_TYPE)swaddr_read((swaddr_t)reg_l(R_EDI), R_ES, DATA_BYTE);
    DATA_TYPE val = x - y;
    DATA_TYPE_S x_s = x;
    DATA_TYPE_S y_s = y;
    DATA_TYPE_S val_s = x_s - y_s;
    // update flags
    cpu.eflags.CF = (uint8_t)(val > x);
    cpu.eflags.OF = (uint8_t)((x_s < 0 && y_s > 0 && val_s >= 0) ||
                              (x_s > 0 && y_s < 0 && val_s <= 0));
    update_PF_ZF_SF((uint32_t)val, DATA_BYTE);
    // update $edi
    if (cpu.eflags.DF) {
        reg_l(R_EDI) -= DATA_BYTE;
    } else {
        reg_l(R_EDI) += DATA_BYTE;
    }
    print_asm(__func__);
    return 1;
}


#include "cpu/exec/template-end.h"
