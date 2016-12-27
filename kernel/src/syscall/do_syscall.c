#include "irq.h"
#include <sys/syscall.h>

void serial_printc(char);
void add_irq_handle(int, void (*)(void));
uint32_t mm_brk(uint32_t);
int fs_ioctl(int, uint32_t, void *);

int fs_open(const char *pathname, int flags);
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, void *buf, int len);
int fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);

static void sys_brk(TrapFrame *tf) {
	tf->eax = mm_brk(tf->ebx);
}

static void sys_ioctl(TrapFrame *tf) {
	tf->eax = fs_ioctl(tf->ebx, tf->ecx, (void *)tf->edx);
}

static void sys_write(TrapFrame *tf) {
	int fd = tf->ebx;
	uint8_t *buf = (void *)tf->ecx;
	int len = tf->edx;
	tf->eax = (uint32_t)fs_write(fd, buf, len);
	return;
}

static void sys_read(TrapFrame *tf) {
	int fd = tf->ebx;
	uint8_t *buf = (void *)tf->ecx;
	int len = tf->edx;
	tf->eax = (uint32_t)fs_read(fd, buf, len);
	return;
}

static void sys_open(TrapFrame *tf) {
	tf->eax = (uint32_t)fs_open((char *)tf->ebx, tf->ecx);
}

static void sys_close(TrapFrame *tf) {
	tf->eax = (uint32_t)fs_close(tf->ebx);
}

static void sys_lseek(TrapFrame *tf) {
	tf->eax = (uint32_t)fs_lseek(tf->ebx, tf->ecx, tf->edx);
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The `add_irq_handle' system call is artificial. We use it to
		 * let user program register its interrupt handlers. But this is
		 * very dangerous in a real operating system. Therefore such a
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;
		case SYS_ioctl: sys_ioctl(tf); break;
		case SYS_write: sys_write(tf); break;
		case SYS_read: sys_read(tf); break;
		case SYS_open: sys_open(tf); break;
		case SYS_close: sys_close(tf); break;
		case SYS_lseek: sys_lseek(tf); break;

		default: panic("Unhandled system call: id = %d, eip = 0x%08x", tf->eax, tf->eip);
	}
}

