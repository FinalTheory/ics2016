#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char *sregsw[] = {"es", "cs", "ss", "ds", "fs", "gs"};

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);
}

static inline
int count_bits(uint8_t v) {
	int i, res = 0;
	for (i = 0; i < 8; i++) {
		if ((v >> i) & 0x01) { res++; }
	}
	return res;
}

int uadd_ok(uint32_t x, uint32_t y) {
	uint32_t sum = x + y;
	return sum >= x;
}

int tadd_ok(int32_t x, int32_t y) {
	int32_t sum = x + y;
	int neg_over = x < 0 && y < 0 && sum >= 0;
	int pos_over = x >= 0 && y >= 0 && sum < 0;
	return !neg_over && !pos_over;
}

void update_PF_ZF_SF(uint32_t res, int data_byte) {
	if (data_byte != 1 &&
		data_byte != 2 &&
		data_byte != 4) {
		Assert(false, "DATA_TYPE should be 1, 2 or 4.");
	}
	int data_bits = data_byte * 8;
	// PF
	cpu.eflags.PF = (uint8_t)!(count_bits((uint8_t)res) % 2);
	// ZF
	cpu.eflags.ZF = (uint8_t)(res == 0);
	// SF
	cpu.eflags.SF = (uint8_t)((res >> (data_bits - 1)) & 0x01);
}
