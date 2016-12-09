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

make_helper(mov_r2cr0) {
    int len = decode_rm_l(eip + 1);
    cpu.cr0.val = op_src->val;
    print_asm("mov %s, %%cr0", op_src->str);
    return len + 1;
}

make_helper(mov_cr02r) {
    int len = decode_rm_l(eip + 1);
    write_operand_l(op_src, cpu.cr0.val);
    print_asm("mov %%cr0, %s", op_src->str);
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
