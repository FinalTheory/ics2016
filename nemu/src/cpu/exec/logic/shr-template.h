#include "cpu/exec/template-start.h"

#define instr shr

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest >>= count;
	OPERAND_W(op_dest, dest);
	update_PF_ZF_SF(dest, DATA_BYTE);
	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
