#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "trap.h"

char buf[] = "Now use \"write\" syscall.\n";

int main() {
	int l, fd = 1;
	printf("%s", "Hello world!\n");
	printf("%d + %d = %d\n", 1, 1, 2);
	printf("Successful wrote %d bytes to fd = %d.\n", l = write(fd, buf, strlen(buf)), fd);
	fd = 3;
	printf("Failed, wrote %d bytes to fd = %d.\n", write(fd, buf, strlen(buf)), fd);
	nemu_assert(l == strlen(buf));
	return 0;
}
