#include "cpu/exec/helper.h"


// TODO: implement ret far ???


// TODO: review this!
#define make_reti_helper(name, DATA_BYTE) make_helper(name) { \
    decode_i_w(eip + 1); \
    uint16_t imm = (uint16_t)op_src->val; \
    cpu.eip = swaddr_read((swaddr_t)cpu.esp, DATA_BYTE); \
    cpu.esp += DATA_BYTE; \
    cpu.esp += imm * (DATA_BYTE / 2); \
    print_asm("ret %s", op_src->str); \
    return 1; \
}

#define make_ret_helper(name, DATA_BYTE) make_helper(name) { \
    cpu.eip = swaddr_read((swaddr_t)cpu.esp, DATA_BYTE); \
    cpu.esp += DATA_BYTE; \
    print_asm("ret"); \
    return 1; \
}

make_reti_helper(ret_i_w, 2)

make_reti_helper(ret_i_l, 4)

make_ret_helper(ret_w, 2)

make_ret_helper(ret_l, 4)

make_helper_v(ret_i)

make_helper_v(ret)
