#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "x86-inc/mmu.h"
#include <stdlib.h>

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	return read_L1(addr, len) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	write_L1(addr, len, data);
}

typedef union {
	struct {
		uint32_t offset : 12;
		uint32_t page : 10;
		uint32_t dir : 10;
	};
	struct {
		uint32_t pad0 : 12;
		uint32_t tag : 20;
	};
	lnaddr_t addr;
} lnaddr_trans_t;


static struct {
	uint32_t tag : 20;
	uint32_t valid : 1;
	PTE entry;
} TLB_cache[TLB_SIZE];

static uint64_t TLB_total = 0, TLB_hit = 0;

void TLB_get_stat(uint64_t *hit, uint64_t *total) {
	*hit = TLB_hit;
	*total = TLB_total;
};

void TLB_flush() {
	memset(TLB_cache, 0, sizeof(TLB_cache));
}

static inline
PTE TLB_mem_translate(lnaddr_t addr) {
	PDE pde;
	PTE pte, invalid = { .val = 0 };
	lnaddr_trans_t trans;
	trans.addr = addr;
	// Because pages are located on 4K boundaries,
	// the low-order 12 bits are always zero
	hwaddr_t page_dir_base = cpu.cr3.page_directory_base << PAGE_OFFSET;
	hwaddr_t page_dir_entry = (trans.dir << 2) + page_dir_base;
	pde.val = hwaddr_read(page_dir_entry, 4);
	if (!pde.present) { goto err; }
	hwaddr_t page_tbl_base = pde.page_frame << PAGE_OFFSET;
	hwaddr_t page_tbl_entry = (trans.page << 2) + page_tbl_base;
	pte.val = hwaddr_read(page_tbl_entry, 4);
	if (!pte.present) { goto err; }
	return pte;
err:
	return invalid;
}

static inline
PTE TLB_search(lnaddr_t addr, int *present) {
	TLB_total++;
	int i;
	lnaddr_trans_t trans = { .addr = addr };
	uint32_t tag = trans.tag;
	for (i = 0; i < TLB_SIZE; i++) {
		if (TLB_cache[i].valid &&
				TLB_cache[i].tag == tag) {
			TLB_hit++;
			if (present) { *present = 1; }
			assert(TLB_cache[i].entry.present);
			return TLB_cache[i].entry;
		}
	}
	// If not found, load it from memory
	PTE entry = TLB_mem_translate(addr);
	i = rand() % TLB_SIZE;
	TLB_cache[i].valid = true;
	TLB_cache[i].tag = tag;
	TLB_cache[i].entry = entry;
	// Error handling
	if (present) {
		*present = entry.present;
	} else {
		Assert(entry.present, "Page directory/table entry is not present!\n");
	}
	return entry;
}

hwaddr_t page_translate(lnaddr_t addr, int *present) {
	PTE entry = TLB_search(addr, present);
	lnaddr_trans_t trans = { .addr = addr };
	return (entry.page_frame << PAGE_OFFSET) | trans.offset;
}

static inline void
check_page_boundary(lnaddr_t addr, size_t len) {
	uint32_t mask = ~((uint32_t)PAGE_MASK);
	if ((addr & mask) != ((addr + len - 1) & mask)) {
		fprintf(stderr, "Address 0x%08x with size=%zu "
						"across page boundary.\n", addr, len);
		assert(false);
	}
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	check_page_boundary(addr, len);
	if (cpu.cr0.paging) {
		return hwaddr_read(page_translate(addr, NULL), len);
	} else {
		return hwaddr_read(addr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	check_page_boundary(addr, len);
	if (cpu.cr0.paging) {
		hwaddr_write(page_translate(addr, NULL), len, data);
	} else {
		hwaddr_write(addr, len, data);
	}
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
	// Granularity bit specifies the units with which the LIMIT field is interpreted.
	// When the bit is clear, the limit is interpreted in units of one byte;
	// when set, the limit is interpreted in units of 4 Kilobytes (a page).
	// Reference: section 5.1.1 of i386 manual
	if (desc_entry.granularity) {
		cpu.sreg_cache[sreg].limit <<= PAGE_OFFSET;
	}
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
