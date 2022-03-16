#include <quadmath.h>
#include <stdio.h>

int main(void) {
    __float128 f = 1.23456;
    char buf[128];

    snprintf(buf, 128, "%g", f);
    printf("snprintf=%s\n", buf);

    quadmath_snprintf(buf, 128, "%Qg", f);
    printf("snprintf=%s\n", buf);

    return 0;
}
