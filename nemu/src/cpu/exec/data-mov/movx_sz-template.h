#include "cpu/exec/template-start.h"

#if DEST_BYTE == 1

#define DEST_SUFFIX b
#define DEST_TYPE uint8_t
#define DEST_TYPE_S int8_t

#elif DEST_BYTE == 2

#define DEST_SUFFIX w
#define DEST_TYPE uint16_t
#define DEST_TYPE_S int16_t

#elif DEST_BYTE == 4

#define DEST_SUFFIX l
#define DEST_TYPE uint32_t
#define DEST_TYPE_S int32_t

#else

#error unknown DEST_BYTE

#endif

#define DEST_REG(index) concat(regs, DEST_SUFFIX)[index]

#define instr movsx

make_helper(concat5(instr, _rm2r_, SUFFIX, _, DEST_SUFFIX)) {
    int len = concat(decode_rm2r_, SUFFIX) (eip + 1);
    DEST_TYPE_S val = (DATA_TYPE_S)op_src->val;
    concat(reg_, DEST_SUFFIX) (op_dest->reg) = (DEST_TYPE)val;
    print_asm(str(instr) str(SUFFIX) str(DEST_SUFFIX) " %s,%%%s",
              op_src->str, DEST_REG(op_dest->reg));
    return len + 1;
}

#undef instr


#define instr movzx

make_helper(concat5(instr, _rm2r_, SUFFIX, _, DEST_SUFFIX)) {
    int len = concat(decode_rm2r_, SUFFIX) (eip + 1);
    DEST_TYPE val = (DATA_TYPE)op_src->val;
    concat(reg_, DEST_SUFFIX) (op_dest->reg) = val;
    print_asm(str(instr) str(SUFFIX) str(DEST_SUFFIX) " %s,%%%s",
              op_src->str, DEST_REG(op_dest->reg));
    return len + 1;
}

#undef instr

#undef DEST_SUFFIX
#undef DEST_TYPE
#undef DEST_TYPE_S

#include "cpu/exec/template-end.h"
