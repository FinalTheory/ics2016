#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE res = src + 1;
	DATA_TYPE_S src_s = op_src->val;
	DATA_TYPE_S res_s = src_s + 1;
	OPERAND_W(op_src, res);
	// update flags
	cpu.eflags.CF = (uint8_t)(res < src);
	cpu.eflags.OF = (uint8_t)(src_s > 0 && res_s < 0);
	update_PF_ZF_SF((uint32_t)res, DATA_BYTE);
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
