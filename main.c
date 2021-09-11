#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

#include <limits.h>

int main() {
    int x = 1;
    int int_size = (sizeof(int) * 8);
    char *buffer = calloc(int_size + 1, sizeof(char ));
    int_to_bin(x, buffer, int_size);
    printf("int: %d bytes: %s\n", x, buffer);
    x = -1;
    int_to_bin(x, buffer, int_size);
    printf("int: %d bytes: %s\n", x, buffer);

    x = -2;
    int_to_bin(x, buffer, int_size);
    printf("int: %d bytes: %s\n", x, buffer);

    x = INT_MAX;
    int_to_bin(x, buffer, int_size);
    printf("Int MAX int: %d bytes: %s\n", x, buffer);

    x = INT_MIN;
    int_to_bin(x, buffer, int_size);
    printf("INT MINint: %d bytes: %s\n", x, buffer);

    // string to int
    char number_str[10] = "12";
    int number = atoi(number_str);
    printf("number %s is number: %d", number_str, number);

    return 0;
}

