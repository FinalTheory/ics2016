#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "mov-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(mov_i2r)
make_helper_v(mov_i2rm)
make_helper_v(mov_r2rm)
make_helper_v(mov_rm2r)
make_helper_v(mov_a2moffs)
make_helper_v(mov_moffs2a)

make_helper(mov_r2cr) {
    int len = decode_rm2r_l(eip + 1);
    switch (op_dest->reg) {
        case 0:
            cpu.cr0.val = op_src->val;
        break;
        case 3:
            cpu.cr3.val = op_src->val;
            // Flush TLB cache at context switch
            TLB_flush();
        break;
        default:
            Assert(false, "Unknown control register.");
    }
    print_asm("mov %s, %%cr%d", op_src->str, op_dest->reg);
    return len + 1;
}

make_helper(mov_cr2r) {
    int len = decode_r2rm_l(eip + 1);
    switch (op_dest->reg) {
        case 0:
            write_operand_l(op_dest, cpu.cr0.val);
        break;
        case 3:
            write_operand_l(op_dest, cpu.cr3.val);
        break;
        default:
            Assert(false, "Unknown control register.");
    }
    print_asm("mov %%cr%d, %s", op_src->reg, op_dest->str);
    return len + 1;
}

make_helper(mov_rm2sreg) {
    int len = decode_rm2r_w(eip + 1);
    uint16_t val = (uint16_t)op_src->val;
    uint8_t sreg = op_dest->sreg;
    update_sreg_cache(sreg, val);
    sreg_w(sreg) = val;
    print_asm("mov %s, %%%s", op_src->str, sreg_name(op_dest->sreg));
    return len + 1;
}

make_helper(mov_sreg2rm) {
    int len = decode_r2rm_w(eip + 1);
    uint16_t val = sreg_w(op_src->sreg);
    write_operand_w(op_dest, val);
    print_asm("mov %%%s, %s", sreg_name(op_src->sreg), op_dest->str);
    return len + 1;
}
