#include "helper.h"
#include <stdlib.h>

void (*int_to_bin_fun)(int, char*, int) = NULL;

int is_little_endian() {
    int x = 1;
    int first_bit = (int) ((char *)&x)[0];
    return first_bit == 1;
}

void int_to_bin_little_endian(int a, char *buffer, int int_size) {
    for (int i = 0; i < int_size; ++i) {
        *buffer++ = (a & 1) + '0';

        a >>= 1;
    }
}

void int_to_bin_big_endian(int a, char *buffer, int int_size) {
    buffer += (int_size - 1);

    for (int i = int_size - 1; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }
}

void int_to_bin(int a, char *buffer, int int_size) {
    if (int_to_bin_fun == NULL) {
        if (is_little_endian())
            int_to_bin_fun = &int_to_bin_little_endian;
        else
            int_to_bin_fun = &int_to_bin_big_endian;
    }

    int_to_bin_fun(a, buffer, int_size);
}
