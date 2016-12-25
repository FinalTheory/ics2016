#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    DATA_TYPE val = (DATA_TYPE)op_src->val;
    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, R_SS,
                 DATA_BYTE, (uint32_t)val);
    print_asm_template1();
}

make_instr_helper(i)
make_instr_helper(r)
make_instr_helper(rm)

make_helper(concat(push_si_, SUFFIX)) {
    int len = decode_si_b(eip + 1);
    DATA_TYPE_S val = (DATA_TYPE_S)op_src->val;
    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, R_SS,
                 DATA_BYTE, (uint32_t)val);
    print_asm_template1();
    return len + 1;
}

make_helper(concat(pusha_, SUFFIX)) {
    int i;
    int regs[] = {R_EAX, R_ECX, R_EDX, R_EBX,
                  R_ESP, R_EBP, R_ESI, R_EDI};
    const int num_regs = sizeof(regs) / sizeof(int);
    DATA_TYPE data[num_regs];
    // Backup all registers
    for (i = 0; i < num_regs; i++) {
        data[i] = (DATA_TYPE)reg_l(regs[i]);
    }
    // Write all data into stack
    for (i = 0; i < num_regs; i++) {
        cpu.esp -= DATA_BYTE;
        swaddr_write((swaddr_t)cpu.esp, R_SS,
                     DATA_BYTE, (uint32_t)data[i]);
    }
    print_asm("pusha" str(SUFFIX));
    return 1;
}

#include "cpu/exec/template-end.h"
