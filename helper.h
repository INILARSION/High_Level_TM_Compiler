#pragma once

void int_to_bin(int a, char *buffer, int int_size);

int int_length(int number);

struct tape {
    int length;
    int *tape_arr;
};

struct tapes {
    // list of all tape structs
    struct tape **tapes;
    int tape_count;
};

struct delta {
    int state_name;
    int *read_symbols;
    int subsequent_state;
    int *write_symbols;
    char *movements;
};

struct delta_group {
    struct delta **deltas;
    int delta_count;
};

struct add {
    int augend_band;
    int addend_band;
    int sum_band;
};

struct sub {
    int minuend_band;
    int subtrahend_band;
    int difference_band;
};

struct incr {
    int incr_band;
};

struct decr {
    int decr_band;
};

struct jump {
    int jump_line_number;
};

struct conditional_jump {
    int band_1;
    int band_2;
    int jump_line_true;
    int jump_line_false;
    char *compare_op;
};

struct line {
    // type of the line
    // types: 'a'=add, 's'=subtract, 'i'=increase, 'd'=decrease, 'c'=conditional jump, 'g'=goto, 'e'=empty
    char type;
    // line number
    int line_number;
    // This points to the type corresponding operation struct
    void *operation_struct;
    // compiled deltas for this line
    struct delta_group *deltas;
};

struct init_variables {
    int variable_count;
    char **variable_names;
    int *variable_init_values;
};

struct program {
    // number of heads/bands
    int head_count;
    // list of state name strings
    char **state_names;
    // number of states
    int state_count;
    // contains the alphabet chars.
    // The alphabet contains ' ', '0', '1'
    char *alphabet;
    // Size of the alphabet. Is 3 for this compiler.
    int alphabet_size;
    // List of all lines of the input program
    struct line **lines;
    int line_count;
    int program_start_line;
    // initial variables and their values
    struct init_variables *init_variables;
};