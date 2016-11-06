#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    DATA_TYPE val = (DATA_TYPE)op_src->val;
    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, DATA_BYTE, (uint32_t)val);
    print_asm_template1();
}

make_instr_helper(i)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
