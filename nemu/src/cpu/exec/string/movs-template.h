#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat3(instr, _, SUFFIX)) {
    DATA_TYPE val = (DATA_TYPE)swaddr_read((swaddr_t)reg_l(R_ESI), DATA_BYTE);
    swaddr_write((swaddr_t)reg_l(R_EDI), DATA_BYTE, val);
    // update $esi, $edi
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
