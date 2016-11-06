#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/*
 * The register encoding scheme is in i386 instruction format.
 * For example, if we access cpu.gpr[3]._16, we will get the `bx' register;
 * if we access cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */
typedef struct {
	union {
		union {
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];

		struct {
			/* Do NOT change the order of the GPRs' definitions. */
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
	uint32_t eip;
	union {
		struct {
			// Carry flag
			uint8_t CF			:1;
			uint8_t unused0		:1;
			// Parity flag
			uint8_t PF			:1;
			uint8_t unused1		:1;
			uint8_t unused2		:1;
			uint8_t unused3		:1;
			// Zero flag
			uint8_t ZF			:1;
			// Sign flag
			uint8_t SF			:1;
			uint8_t unused4		:1;
			// Interrupt enable flag
			uint8_t IF			:1;
			// For MOVS, CMPS, SCAS, LODS and STOS
			uint8_t DF			:1;
			// Overflow flag
			uint8_t OF			:1;
		};
		uint32_t val;
	} eflags;
} CPU_state;

extern CPU_state cpu;


// Update CF and OF manually!
void update_PF_ZF_SF(uint32_t, int);

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
