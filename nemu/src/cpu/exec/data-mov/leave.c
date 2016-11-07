#include "cpu/exec/helper.h"
#include "leave.h"

make_helper(leave) {
    cpu.esp = cpu.ebp;
    cpu.ebp = swaddr_read((swaddr_t)cpu.esp, sizeof(cpu.ebp));
    cpu.esp += sizeof(cpu.ebp);
    print_asm("leave");
    return 1;
}
