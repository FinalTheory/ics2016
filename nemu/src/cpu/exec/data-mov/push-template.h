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

#include "cpu/exec/template-end.h"
