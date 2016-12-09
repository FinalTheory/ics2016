#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(lgdt) {
	ModR_M m;
	m.val = (uint8_t)instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	swaddr_t addr = op_src->addr;
	cpu.gdtr.limit = (uint16_t)lnaddr_read(addr, 2);
	cpu.gdtr.base = lnaddr_read(addr + 2, 4);
	print_asm("lgdt %s", op_src->str);
	return 1 + len;
}
