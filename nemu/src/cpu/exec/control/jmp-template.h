#include "cpu/exec/template-start.h"

#define instr jmprel

static void do_execute() {
    int32_t rel = (DATA_TYPE_S)op_src->val;
    // update $eip
    cpu.eip += rel;
#if DATA_BYTE == 2
    cpu.eip &= 0x0000ffffu;
#endif
    print_asm_template1();
}

make_instr_helper(i)

#undef instr

#if DATA_BYTE == 2 || DATA_BYTE == 4

#define instr jmprm

make_helper(concat3(instr, _rm_, SUFFIX)) {
    concat3(decode_, rm_, SUFFIX)(eip + 1);
    DATA_TYPE val = (DATA_TYPE)op_src->val;
    cpu.eip = (uint32_t)val - ops_decoded.is_operand_size_16;
    print_asm_template1();
    return 0;
}

#undef instr

#endif

#include "cpu/exec/template-end.h"
