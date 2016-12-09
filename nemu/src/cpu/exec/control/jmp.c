#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jmp-template.h"
#undef DATA_BYTE

make_helper_v(jmprel_i)

make_helper_v(jmprm_rm)

#define LJMP_SIZE 7

make_helper(ljmp) {
    uint32_t addr = instr_fetch(eip + 1, 4);
    uint16_t cs = (uint16_t)instr_fetch(eip + 5, 2);
    cpu.cs = cs;
    cpu.eip = addr - LJMP_SIZE;
    update_sreg_cache(R_CS, cs);
    print_asm("ljmp CS: 0x%04x  IP: 0x%08x\n", cs, addr);
    return LJMP_SIZE;
}
