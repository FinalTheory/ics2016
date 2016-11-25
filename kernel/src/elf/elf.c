#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
  int i, t = 0;
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif

	elf = (void*)buf;

	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);

	/* Load each program segment */
	// TODO: add log here.
	for(i = 0 ; i < elf->e_phnum; i++) {
    ph = (Elf32_Phdr *)(buf + elf->e_phoff) + i;
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {
      t++;
      ramdisk_read((uint8_t*)ph->p_vaddr,
                   ELF_OFFSET_IN_DISK + ph->p_offset,
                   ph->p_filesz);
      nemu_assert(ph->p_memsz >= ph->p_filesz);
      memset((void*)ph->p_vaddr + ph->p_filesz,
             0, ph->p_memsz - ph->p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
		}
	}
  nemu_assert(t > 0);

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
