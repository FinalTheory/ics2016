#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
    // do calc
    DATA_TYPE x = (DATA_TYPE)op_dest->val;
    DATA_TYPE y = (DATA_TYPE)op_src->val;
    DATA_TYPE val = x - y;

    DATA_TYPE_S x_s = (DATA_TYPE_S)op_dest->val;
    DATA_TYPE_S y_s = (DATA_TYPE_S)op_src->val;
    DATA_TYPE_S val_s = x_s - y_s;
    
    // update flags
    cpu.eflags.CF = (uint8_t)(val > x);
    cpu.eflags.OF = (uint8_t)((x_s < 0 && y_s > 0 && val_s > 0) ||
                              (x_s > 0 && y_s < 0 && val_s < 0));
    update_PF_ZF_SF(val, DATA_BYTE);
    OPERAND_W(op_dest, val);
    print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
