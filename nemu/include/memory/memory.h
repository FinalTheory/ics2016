#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define HW_MEM_SIZE (128 * 1024 * 1024)

extern uint8_t *hw_mem;

/* convert the hardware address in the test program to virtual address in NEMU */
#define hwa_to_va(p) ((void *)(hw_mem + (unsigned)p))
/* convert the virtual address in NEMU to hardware address in the test program */
#define va_to_hwa(p) ((hwaddr_t)((void *)p - (void *)hw_mem))

#define hw_rw(addr, type) *(type *)({\
	Assert(addr < HW_MEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
	hwa_to_va(addr); \
})

uint32_t swaddr_read(swaddr_t, uint8_t, size_t);
uint32_t lnaddr_read(lnaddr_t, size_t);
uint32_t hwaddr_read(hwaddr_t, size_t);
void swaddr_write(swaddr_t, uint8_t, size_t, uint32_t);
void lnaddr_write(lnaddr_t, size_t, uint32_t);
void hwaddr_write(hwaddr_t, size_t, uint32_t);

lnaddr_t seg_translate(swaddr_t, uint8_t);
void update_sreg_cache(uint8_t, uint16_t);

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

void init_L1();
uint32_t read_L1(hwaddr_t addr, size_t len);
void write_L1(hwaddr_t addr, size_t len, uint32_t data);
int debug_search_L1(hwaddr_t addr);
void get_stat_L1(uint64_t *hit, uint64_t *total);

void init_L2();
uint32_t read_L2(hwaddr_t addr, size_t len);
void write_L2(hwaddr_t addr, size_t len, uint32_t data);
int debug_search_L2(hwaddr_t addr);
void get_stat_L2(uint64_t *hit, uint64_t *total);

hwaddr_t page_translate(lnaddr_t addr, int *present);
void TLB_flush();
void TLB_get_stat(uint64_t *hit, uint64_t *total);

#endif
