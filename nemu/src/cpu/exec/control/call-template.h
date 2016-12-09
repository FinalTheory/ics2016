#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    int32_t rel = (DATA_TYPE_S)op_src->val;
    // push $eip
    cpu.esp -= DATA_BYTE;
    // Notice that here we save DATA_BYTE + $eip
    // this address is 1 or 2 byte before next instruction
    // if operand size is 16, then "0F" prefix would also before "ret"
    // which would advance $eip by 2, just to correct next instruction
    swaddr_write((swaddr_t)cpu.esp, R_SS, DATA_BYTE,
                 (uint32_t)(cpu.eip + DATA_BYTE));
    // update $eip
    cpu.eip += rel;
#if DATA_BYTE == 2
    cpu.eip &= 0x0000ffffu;
#endif
    print_asm_template1();
}

make_instr_helper(i)

make_helper(concat3(instr, _rm_, SUFFIX)) {
    // decode the instruction
    int len = concat3(decode_, rm_, SUFFIX)(eip + 1);
    // push $eip
    cpu.esp -= DATA_BYTE;
    swaddr_write((swaddr_t)cpu.esp, R_SS, DATA_BYTE,
                 (uint32_t)(cpu.eip + len));
    // update $eip
    DATA_TYPE val = (DATA_TYPE)op_src->val;
    cpu.eip = (uint32_t)val - ops_decoded.is_operand_size_16;
#if DATA_BYTE == 2
    cpu.eip &= 0x0000ffffu;
#endif
    print_asm_template1();
    return 0;
}

#include "cpu/exec/template-end.h"
