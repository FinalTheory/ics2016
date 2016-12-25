#include "trap.h"

int a(int n1, int n2, int n3, int n4) {
	// set_bp();
	return n1 + 1;
}

int b(int n1, int n2, int n3, int n4) {
	return a(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
}

int c(int n1, int n2, int n3, int n4) {
	return b(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
}

int d(int n1, int n2, int n3, int n4) {
	return c(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
}

int e(int n1, int n2, int n3, int n4) {
	return d(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
}

int f(int n1, int n2, int n3, int n4) {
	return e(n1 + 1, n2 + 1, n3 + 1, n4 + 1);
}

int main() {
	nemu_assert(f(0, 1, 2, 3) == 6);
	return 0;
}
