#include <nemu.h>
#include "cpu/exec/helper.h"

#define make_setcc_func(name, cond) make_helper(name) { \
    int len = decode_rm_b(eip + 1); \
    write_operand_b(op_src, (cond)); \
    print_asm("%s %s", __func__, op_src->str); \
    return len + 1; \
}

make_setcc_func(seta, !cpu.eflags.CF && !cpu.eflags.ZF)

make_setcc_func(setae, !cpu.eflags.CF)

make_setcc_func(setb, cpu.eflags.CF)

make_setcc_func(setbe, cpu.eflags.CF || cpu.eflags.ZF)

make_setcc_func(sete, cpu.eflags.ZF)

make_setcc_func(setg, cpu.eflags.SF == cpu.eflags.OF && !cpu.eflags.ZF)

make_setcc_func(setge, cpu.eflags.SF == cpu.eflags.OF)

make_setcc_func(setl, cpu.eflags.SF != cpu.eflags.OF)

make_setcc_func(setle, cpu.eflags.SF != cpu.eflags.OF || cpu.eflags.ZF)

make_setcc_func(setne, !cpu.eflags.ZF)

make_setcc_func(setno, !cpu.eflags.OF)

make_setcc_func(setnp, !cpu.eflags.PF)

make_setcc_func(setns, !cpu.eflags.SF)

make_setcc_func(seto, cpu.eflags.OF)

make_setcc_func(setp, cpu.eflags.PF)

make_setcc_func(sets, cpu.eflags.SF)
