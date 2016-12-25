#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)

static PTE memtable[NR_PTE] align_to_page;

/* Use the function to get the start address of user page directory. */
PDE* get_updir();

void create_video_mapping() {
	/* Create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area 
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	PDE *updir = get_updir();
	memset(memtable, 0, sizeof(memtable));
	// Fill the page directory
	// Notice that we only need to translate virtual address
	// to physical address here, since kernel address space
	// is already configured completely.
	updir[VMEM_ADDR / PT_SIZE].val = make_pde(va_to_pa(memtable));
	uint32_t pframe_addr;
	int i = VMEM_ADDR / PAGE_SIZE;
	for (pframe_addr = VMEM_ADDR;
			 pframe_addr < VMEM_ADDR + SCR_SIZE;
			 pframe_addr += PAGE_SIZE) {
		memtable[i++].val = make_pte(pframe_addr);
	}
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

