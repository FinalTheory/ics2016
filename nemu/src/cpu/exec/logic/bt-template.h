#include "cpu/exec/template-start.h"

#define instr bt

static void do_execute () {
	cpu.eflags.CF = ((op_dest->val) >> (op_src->val)) & 0x01;
	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
