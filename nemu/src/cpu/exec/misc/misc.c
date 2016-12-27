#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "x86-inc/mmu.h"
#include <setjmp.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

extern jmp_buf jbuf;

make_helper(nop) {
	print_asm("nop");
	return 1;
}

void raise_intr(uint8_t NO) {
	/*
	 * Trigger an interrupt/exception with "NO",
	 * which is, use "NO" to index the IDT and call the handler.
	 */
	assert(sizeof(cpu.eflags) == 4);
	assert(sizeof(GateDesc) == 2 * sizeof(uint32_t));

	/* push EFLAGS, CS, EIP */
	cpu.esp -= sizeof(cpu.eflags);
	swaddr_write((swaddr_t)cpu.esp, R_SS,
							 sizeof(cpu.eflags), cpu.eflags.val);

	cpu.esp -= sizeof(cpu.cs);
	swaddr_write((swaddr_t)cpu.esp, R_SS,
							 sizeof(cpu.cs), cpu.cs);

	cpu.esp -= sizeof(cpu.eip);
	swaddr_write((swaddr_t)cpu.esp, R_SS,
							 sizeof(cpu.eip), cpu.eip);

	/* Load gate descriptor from IDT */
	GateDesc desc;
	lnaddr_t desc_addr = cpu.idtr.base + sizeof(GateDesc) * NO;
	((uint32_t *)&desc)[0] = lnaddr_read(desc_addr, 4);
	((uint32_t *)&desc)[1] = lnaddr_read(desc_addr + 4, 4);

	/* Update CS */
	cpu.cs = (uint16_t)desc.segment;
	update_sreg_cache(R_CS, cpu.cs);

	/* Update EIP */
	swaddr_t offset = (desc.offset_31_16 << 16) | desc.offset_15_0;
	lnaddr_t eip = seg_translate(offset, R_CS);
	cpu.eip = eip;

	/* Jump back to cpu_exec() */
	longjmp(jbuf, 1);
}

make_helper(iret) {
	assert(!ops_decoded.is_operand_size_16);
	/* pop EIP, CS, EFLAGS */
	cpu.eip = swaddr_read((swaddr_t)cpu.esp, R_SS, sizeof(cpu.eip));
	cpu.esp += sizeof(cpu.eip);

	cpu.cs = (uint16_t)swaddr_read((swaddr_t)cpu.esp, R_SS, sizeof(cpu.cs));
	cpu.esp += sizeof(cpu.cs);

	cpu.eflags.val = swaddr_read((swaddr_t)cpu.esp, R_SS, sizeof(cpu.eflags));
	cpu.esp += sizeof(cpu.eflags);

	print_asm("iret");

	// Here we return 0 because the updated EIP
	// points exactly to next instruction after "int"
	return 0;
}

make_helper(_int) {
	assert(!ops_decoded.is_operand_size_16);
	int len = decode_i_b(eip + 1);
	uint8_t NO = (uint8_t)op_src->val;
	print_asm("int %d", NO);
	// Here we should first update eip,
	// thus the address of next instruction would be pushed into
	// stack and then as return address from interrupt handler.
	cpu.eip += len + 1;
	raise_intr(NO);
	// This return is never used
	assert(false);
	return 0;
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

make_helper(lidt) {
	ModR_M m;
	m.val = (uint8_t)instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	swaddr_t addr = op_src->addr;
	cpu.idtr.limit = (uint16_t)lnaddr_read(addr, 2);
	cpu.idtr.base = lnaddr_read(addr + 2, 4);
	print_asm("lidt %s", op_src->str);
	return 1 + len;
}

make_helper(cli) {
	cpu.eflags.IF = 0;
	print_asm("cli");
	return 1;
}

make_helper(sti) {
	cpu.eflags.IF = 1;
	print_asm("sti");
	return 1;
}

useconds_t time_left();
void timer_sig_handler(int signum);
static struct itimerval it = {
				.it_value.tv_sec = 0,
				.it_value.tv_usec = 0,
				.it_interval.tv_sec = 0,
				.it_interval.tv_usec = 0,
};

make_helper(hlt) {
	Assert(cpu.eflags.IF, "Fatal: interrupt is disabled!");
	// Once hlt is called, we manually disable the timer
	int ret = setitimer(ITIMER_VIRTUAL, &it, NULL);
	Assert(ret == 0, "Can not clear old timer.");
	while (!cpu.INTR) {
		// When process is sleeping, "SIGVTALRM" signal would not be delivered,
		// so we should calculate the left time, cancel old timer,
		// sleep for that time, and then manually call signal handler.
		useconds_t t = time_left();
		if (t) {
			ret = usleep(t);
			Assert(ret == 0, "usleep() failed with errno = %d", errno);
		}
		// Then we manually call signal handler
		timer_sig_handler(SIGVTALRM);
		// After this, there should be an interrupt,
		// and this loop shall exit
	}
	return 1;
}
