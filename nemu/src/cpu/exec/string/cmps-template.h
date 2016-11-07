#include "cpu/exec/template-start.h"

#define instr cmps

make_helper(concat3(instr, _, SUFFIX)) {
    DATA_TYPE src = (DATA_TYPE)swaddr_read((swaddr_t)reg_l(R_ESI), DATA_BYTE);
    DATA_TYPE dst = (DATA_TYPE)swaddr_read((swaddr_t)reg_l(R_EDI), DATA_BYTE);
    DATA_TYPE_S src_s = src;
    DATA_TYPE_S dst_s = dst;
    DATA_TYPE val = src - dst;
    DATA_TYPE_S val_s = src_s - dst_s;
    // update flags
    cpu.eflags.CF = (uint8_t)(val > src);
    cpu.eflags.OF = (uint8_t)((src_s < 0 && dst_s > 0 && val_s > 0) ||
                              (src_s > 0 && dst_s < 0 && val_s < 0));
    update_PF_ZF_SF((uint32_t)val, DATA_BYTE);
    // increment $esi, $edi
    if (cpu.eflags.DF) {
        reg_l(R_ESI) -= DATA_BYTE;
        reg_l(R_EDI) -= DATA_BYTE;
    } else {
        reg_l(R_ESI) += DATA_BYTE;
        reg_l(R_EDI) += DATA_BYTE;
    }
    print_asm(str(instr));
    return 1;
}


#include "cpu/exec/template-end.h"
