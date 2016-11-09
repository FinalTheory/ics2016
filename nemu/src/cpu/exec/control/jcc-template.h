#include "cpu/exec/template-start.h"

static void do_execute() {
    if (condition) {
        int32_t rel = (DATA_TYPE_S)op_src->val;
        // update $eip
        cpu.eip += rel;
#if DATA_BYTE == 2
        cpu.eip &= 0x0000ffffu;
#endif
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
