#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    DATA_TYPE val = swaddr_read((swaddr_t)cpu.esp, R_SS, DATA_BYTE);
    cpu.esp += DATA_BYTE;
    OPERAND_W(op_src, val);
    print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)

make_helper(concat3(popa, _, SUFFIX)) {
    int i;
#if DATA_BYTE == 2
#undef reg_access
#define reg_access reg_w
    int regs[] = {R_DI, R_SI, R_BP, -1,
                  R_BX, R_DX, R_CX, R_AX};
#elif DATA_BYTE == 4
#undef reg_access
#define reg_access reg_l
    int regs[] = {R_EDI, R_ESI, R_EBP, -1,
                  R_EBX, R_EDX, R_ECX, R_EAX};
#endif
    const int num_regs = sizeof(regs) / sizeof(int);
    // Pop all data from stack
    for (i = 0; i < num_regs; i++) {
        if (regs[i] != -1)
            reg_access(regs[i]) = swaddr_read((swaddr_t)cpu.esp, R_SS, DATA_BYTE);
        cpu.esp += DATA_BYTE;
    }
    print_asm("popa" str(SUFFIX));
    return 1;
}

#include "cpu/exec/template-end.h"
