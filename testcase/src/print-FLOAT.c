#include <stdio.h>
#include <string.h>
#include "FLOAT.h"

#include "trap.h"

char buf[128];

int main() {
	init_FLOAT_vfprintf();

#ifdef LINUX_RT
	printf("%f\n", FLOAT_ARG(0x00010000));
	printf("%f\n", FLOAT_ARG(0x00013333));
	printf("%f %d\n", FLOAT_ARG(0xfffecccd), 123456);
#else
	sprintf(buf, "%f", FLOAT_ARG(0x00010000));
	nemu_assert(strcmp(buf, "1.000000") == 0);
	
	sprintf(buf, "%f", FLOAT_ARG(0x00013333));
	nemu_assert(strcmp(buf, "1.199996") == 0);

	sprintf(buf, "%f %d", FLOAT_ARG(0xfffecccd), 123456);
	nemu_assert(strcmp(buf, "-1.199996 123456") == 0);

	FLOAT a = f2F(1.2);
	FLOAT b = f2F(0.333333);
	printf("a = %f\n", a);
	printf("b = %f\n", b);
	printf("a * b = %f\n", F_mul_F(a, b));
	printf("a^0.333 = %f\n", pow(a, b));
#endif

	return 0;
}
