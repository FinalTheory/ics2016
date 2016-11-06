#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
    DATA_TYPE_S rel = (DATA_TYPE_S)op_src->val;
    int32_t eip = rel + (int32_t)cpu.eip;
    if (cpu.eflags.ZF) { cpu.eip = eip; }
    print_asm_template1();
}

make_instr_helper(i)

#undef je



#include "cpu/exec/template-end.h"
