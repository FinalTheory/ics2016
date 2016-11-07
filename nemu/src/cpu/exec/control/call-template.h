#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    DATA_TYPE rel = (DATA_TYPE)op_src->val;
    DATA_TYPE eip = (DATA_TYPE)cpu.eip;
    // push $eip
    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, DATA_BYTE, (uint32_t)eip);
    // update $eip
    eip += rel;
    uint32_t mask = (DATA_TYPE)~0;
    cpu.eip = (cpu.eip & (~mask)) | ((uint32_t)eip & mask);

    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
