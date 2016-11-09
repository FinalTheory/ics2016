#include "FLOAT.h"
#include <stdint.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	int64_t res = (int64_t)a * (int64_t)b;
	res >>= SCALE;
	return (FLOAT)res;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	FLOAT res, unused;
	int64_t divisor = (int64_t)a << SCALE;
	uint64_t mask = (uint32_t)-1;
	int32_t high = (divisor & (~mask)) >> 32;
	int32_t low = (divisor & mask);
	asm volatile ("idiv %2" : "=a"(res), "=d"(unused) : "r"(b), "a"(low), "d"(high));
	return res;
}

typedef struct {
	uint32_t frac	: 23;
	uint32_t exp	: 8;
	uint32_t s		: 1;
} float_t;

FLOAT f2F(float a) {
    FLOAT v, f;
    asm volatile ("movl (%1), %0" : "=r"(v) : "r"(&a));
	float_t *p = (void *)&v;
	f = ((p->frac >> 7) + (1 << SCALE)) * (p->s ? -1 : 1);
	int bias = 127, exp;
	if (p->exp != 0) {
		exp = p->exp - bias;
	} else {
		exp = 1 - bias;
	}
	if (exp > 0) {
		f <<= exp;
	} else {
		f >>= -exp;
	}
	return f;
}

FLOAT Fabs(FLOAT a) {
    return a >= 0 ? a : -a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

