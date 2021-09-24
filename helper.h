#pragma once

void int_to_bin(int a, char *buffer, int int_size);

int int_length(int number);

struct delta {
    int heads;
    int state_name;
    int *read_symbols;
    int subsequent_state;
    int *write_symbols;
    char *movements;
};

struct delta_group {
    struct delta **deltas;
    int delta_count;
    int start_state;
    int subsequent_state;
};

struct line {
    // type of the line
    // types: 'a'=add, 's'=subtract, 'i'=increase, 'd'=decrease, 'c'=conditional jump, 'g'=goto
    char type;
    // line number
    int number;
    struct delta_group *deltas;
};

struct program {
    char **state_names;
    int state_count;
    char *alphabet;
    char alphabet_size;
    struct delta *deltas;
    int delta_count;
    int head_count;
};