#include "common.h"
#include "string.h"
#include <sys/ioctl.h>

void serial_printc(char);

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

typedef struct {
	bool opened;
	uint32_t offset;
	uint32_t cursor;
	uint32_t size;
} Fstate;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] __attribute__((used)) = {
				{"1.rpg",        188864,   1048576},
				{"2.rpg",        188864,   1237440},
				{"3.rpg",        188864,   1426304},
				{"4.rpg",        188864,   1615168},
				{"5.rpg",        188864,   1804032},
				{"abc.mkf",      1022564,  1992896},
				{"ball.mkf",     134704,   3015460},
				{"data.mkf",     66418,    3150164},
				{"desc.dat",     16295,    3216582},
				{"fbp.mkf",      1128042,  3232877},
				{"fire.mkf",     834728,   4360919},
				{"f.mkf",        186966,   5195647},
				{"gop.mkf",      11530322, 5382613},
				{"map.mkf",      1496578,  16912935},
				{"mgo.mkf",      1577442,  18409513},
				{"m.msg",        188232,   19986955},
				{"mus.mkf",      331284,   20175187},
				{"pat.mkf",      8488,     20506471},
				{"rgm.mkf",      453202,   20514959},
				{"rng.mkf",      4546074,  20968161},
				{"scrn0001.bmp", 77878,    25514235},
				{"sss.mkf",      557004,   25592113},
				{"voc.mkf",      1997044,  26149117},
				{"wor16.asc",    5374,     28146161},
				{"wor16.fon",    82306,    28151535},
				{"word.dat",     5650,     28233841},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

static Fstate fd_table[NR_FILES + 3];

#define MAKE_FD(i) (i + 3)

#define CHECK_FD if (fd < MAKE_FD(0) || fd >= MAKE_FD(NR_FILES)) { return -1; } \
if (!fd_table[fd].opened) { return -1; } \

int fs_ioctl(int fd, uint32_t request, void *p) {
	assert(request == TCGETS);
	return (fd >= 0 && fd <= 2 ? 0 : -1);
}

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);
typedef void (*ide_func)(uint8_t *, uint32_t, uint32_t);

/* A simplified file system */


int fs_open(const char *pathname, int flags) {
	int i;
	for (i = 0; i < NR_FILES; i++) {
		if (strcmp(pathname, file_table[i].name) == 0) {
			if (!fd_table[MAKE_FD(i)].opened) {
				fd_table[MAKE_FD(i)].opened = true;
				fd_table[MAKE_FD(i)].offset = file_table[i].disk_offset;
				fd_table[MAKE_FD(i)].size = file_table[i].size;
				fd_table[MAKE_FD(i)].cursor = 0;
				return MAKE_FD(i);
			} else {
				break;
			}
		}
	}
	panic("File %s not found.", pathname);
	return -1;
}

static int __fs_helper(ide_func f, int fd, void *buf, int len) {
	if (0 <= fd && fd < 3) { return 0; }
	CHECK_FD
	assert(fd_table[fd].cursor <= fd_table[fd].size);
	if (fd_table[fd].cursor == fd_table[fd].size) {
		return 0;
	}
	int left = fd_table[fd].size - fd_table[fd].cursor;
	if (len > left) { len = left; }
	f(buf, fd_table[fd].offset + fd_table[fd].cursor, len);
	fd_table[fd].cursor += len;
	return len;
}

int fs_read(int fd, void *buf, int len) {
	return __fs_helper(ide_read, fd, buf, len);
}

int fs_write(int fd, void *buf, int len) {
	// Write data to serial port
	// only for stdout and stderr
	if (fd == 1 || fd == 2) {
		int i;
		for (i = 0; i < len; i++) {
			serial_printc(((char *)buf)[i]);
		}
		return len;
	}
	return __fs_helper(ide_write, fd, buf, len);
}

int fs_lseek(int fd, int offset, int whence) {
	CHECK_FD
	uint32_t new_cursor = 0;
	switch (whence) {
		case SEEK_SET:
			new_cursor = (uint32_t)offset;
			break;
		case SEEK_CUR:
			new_cursor = fd_table[fd].cursor + offset;
			break;
		case SEEK_END:
			new_cursor = fd_table[fd].size + offset;
			break;
		default:
			return -1;
	}
	if (new_cursor >= 0 &&
			new_cursor <= fd_table[fd].size) {
		fd_table[fd].cursor = new_cursor;
		return new_cursor;
	} else {
		return -1;
	}
}

int fs_close(int fd) {
	CHECK_FD
	memset(&fd_table[fd], 0, sizeof(fd_table[fd]));
	fd_table[fd].opened = false;
	return 0;
}
