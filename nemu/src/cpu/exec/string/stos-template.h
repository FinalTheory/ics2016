#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat3(instr, _, SUFFIX)) {
    swaddr_write((swaddr_t)reg_l(R_EDI),
                 DATA_BYTE, (DATA_TYPE)cpu.eax);
    // update $esi, $edi
    if (cpu.eflags.DF) {
        reg_l(R_EDI) -= DATA_BYTE;
    } else {
        reg_l(R_EDI) += DATA_BYTE;
    }
    print_asm(str(instr));
    return 1;
}


#include "cpu/exec/template-end.h"
