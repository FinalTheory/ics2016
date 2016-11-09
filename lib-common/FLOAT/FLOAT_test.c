#include "float.h"
#include <stdio.h>

int main() {
    float scale = (1 << 16);
    FLOAT a = 165539;
    FLOAT b = 6573;

    printf("scale = %.4f\n", scale);
    printf("float = %.4f\n", (a / scale) / (b / scale));
    printf("FLOAT = %.4f\n", F_div_F(a, b) / scale);
    printf("float = %.4f\n", (a / scale) * (b / scale));
    printf("FLOAT = %.4f\n", F_mul_F(a, b) / scale);
    float f = 8.732;
    printf("f = %.4f, f2F = %.4f\n", f, f2F(f) / scale);
    f = -1.732;
    printf("f = %.4f, f2F = %.4f\n", f, f2F(f) / scale);
    f = 0.0732;
    printf("f = %.4f, f2F = %.4f\n", f, f2F(f) / scale);
    return 0;
}
