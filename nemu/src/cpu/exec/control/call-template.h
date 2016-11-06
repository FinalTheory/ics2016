#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    DATA_TYPE rel = (DATA_TYPE)op_src->val;
    DATA_TYPE mask = ~((DATA_TYPE)0);
    DATA_TYPE eip = (DATA_TYPE)cpu.eip;

    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, DATA_BYTE, (uint32_t)eip);
    cpu.eip = (cpu.eip + rel) & mask;

    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
