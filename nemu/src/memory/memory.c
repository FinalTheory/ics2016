#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "x86-inc/mmu.h"

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return read_L1(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	write_L1(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

void update_sreg_cache(uint8_t sreg, uint16_t val) {
	SegDesc desc_entry;
	assert(sizeof(SegDesc) == 8);
	uint32_t offset = val & (~0u << 3);
	lnaddr_t desc_addr = cpu.gdtr.base + offset;
	((uint32_t *)&desc_entry)[0] = lnaddr_read(desc_addr, 4);
	((uint32_t *)&desc_entry)[1] = lnaddr_read(desc_addr + 4, 4);
	cpu.sreg_cache[sreg].base = desc_entry.base_15_0 |
								(desc_entry.base_23_16 << 16) |
								(desc_entry.base_31_24 << 24);
	cpu.sreg_cache[sreg].limit = desc_entry.limit_15_0 |
								 (desc_entry.limit_19_16 << 16);
}

lnaddr_t seg_translate(swaddr_t addr, uint8_t sreg) {
	return cpu.sreg_cache[sreg].base + addr;
}

uint32_t swaddr_read(swaddr_t addr, uint8_t sreg, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
    if (cpu.cr0.protect_enable) {
		return lnaddr_read(seg_translate(addr, sreg), len);
	} else {
		return lnaddr_read(addr, len);
	}
}

void swaddr_write(swaddr_t addr, uint8_t sreg, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	if (cpu.cr0.protect_enable) {
		lnaddr_write(seg_translate(addr, sreg), len, data);
	} else {
		lnaddr_write(addr, len, data);
	}
}
