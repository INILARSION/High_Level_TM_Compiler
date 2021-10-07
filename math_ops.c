#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helper.h"

struct delta *start_delta(struct program *program, int state_name, int subsequent_state, char *movements) {
    struct delta *delta = malloc(sizeof(struct delta));
    delta->state_name = state_name;
    // For the add operation 3 new states were introduced. The first of them is the next state
    delta->subsequent_state = subsequent_state;
    delta->read_symbols = malloc(program->head_count * sizeof(int));
    delta->write_symbols = malloc(program->head_count * sizeof(int));
    delta->movements = movements;
    for (int i = 0; i < program->head_count; ++i) {
        delta->read_symbols[i] = 0;
        delta->write_symbols[i] = 0;
    }
    return delta;
}

struct delta_group *create_math_deltas(struct program *program, char *movements, int term_1_band, int term_2_band, int result_band) {
    struct delta_group *delta_group = malloc(sizeof(struct delta_group));
    delta_group->delta_count = 8;
    delta_group->deltas = malloc(delta_group->delta_count * sizeof(struct delta *));
    // set common attributes
    for (int i = 0; i < delta_group->delta_count; ++i) {
        delta_group->deltas[i] = malloc(sizeof(struct delta));
        delta_group->deltas[i]->read_symbols = calloc(program->head_count, sizeof(int));
        delta_group->deltas[i]->write_symbols = calloc(program->head_count, sizeof(int));
        delta_group->deltas[i]->movements = movements;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == result_band) {
            delta_group->deltas[0]->read_symbols[i] = 1;
            delta_group->deltas[1]->read_symbols[i] = 1;
            delta_group->deltas[2]->read_symbols[i] = 1;
            delta_group->deltas[3]->read_symbols[i] = 1;
            delta_group->deltas[4]->read_symbols[i] = 2;
            delta_group->deltas[5]->read_symbols[i] = 2;
            delta_group->deltas[6]->read_symbols[i] = 2;
            delta_group->deltas[7]->read_symbols[i] = 2;
        }
        if (i == term_1_band) {
            delta_group->deltas[0]->read_symbols[i] = 1;
            delta_group->deltas[1]->read_symbols[i] = 1;
            delta_group->deltas[2]->read_symbols[i] = 2;
            delta_group->deltas[3]->read_symbols[i] = 2;
            delta_group->deltas[4]->read_symbols[i] = 1;
            delta_group->deltas[5]->read_symbols[i] = 1;
            delta_group->deltas[6]->read_symbols[i] = 2;
            delta_group->deltas[7]->read_symbols[i] = 2;

            // write the same symbol on the tape as was written
            delta_group->deltas[0]->write_symbols[i] = 1;
            delta_group->deltas[1]->write_symbols[i] = 1;
            delta_group->deltas[2]->write_symbols[i] = 2;
            delta_group->deltas[3]->write_symbols[i] = 2;
            delta_group->deltas[4]->write_symbols[i] = 1;
            delta_group->deltas[5]->write_symbols[i] = 1;
            delta_group->deltas[6]->write_symbols[i] = 2;
            delta_group->deltas[7]->write_symbols[i] = 2;
        } else if (i == term_2_band) {
            delta_group->deltas[0]->read_symbols[i] = 1;
            delta_group->deltas[1]->read_symbols[i] = 2;
            delta_group->deltas[2]->read_symbols[i] = 1;
            delta_group->deltas[3]->read_symbols[i] = 2;
            delta_group->deltas[4]->read_symbols[i] = 1;
            delta_group->deltas[5]->read_symbols[i] = 2;
            delta_group->deltas[6]->read_symbols[i] = 1;
            delta_group->deltas[7]->read_symbols[i] = 2;

            delta_group->deltas[0]->write_symbols[i] = 1;
            delta_group->deltas[1]->write_symbols[i] = 2;
            delta_group->deltas[2]->write_symbols[i] = 1;
            delta_group->deltas[3]->write_symbols[i] = 2;
            delta_group->deltas[4]->write_symbols[i] = 1;
            delta_group->deltas[5]->write_symbols[i] = 2;
            delta_group->deltas[6]->write_symbols[i] = 1;
            delta_group->deltas[7]->write_symbols[i] = 2;
        }
        if (i != term_1_band && i != term_2_band && i != result_band) {
            // every other tape than the terms and result bands should read and write a blank
            delta_group->deltas[0]->read_symbols[i] = 0;
            delta_group->deltas[1]->read_symbols[i] = 0;
            delta_group->deltas[2]->read_symbols[i] = 0;
            delta_group->deltas[3]->read_symbols[i] = 0;
            delta_group->deltas[4]->read_symbols[i] = 0;
            delta_group->deltas[5]->read_symbols[i] = 0;
            delta_group->deltas[6]->read_symbols[i] = 0;
            delta_group->deltas[7]->read_symbols[i] = 0;

            delta_group->deltas[0]->write_symbols[i] = 0;
            delta_group->deltas[1]->write_symbols[i] = 0;
            delta_group->deltas[2]->write_symbols[i] = 0;
            delta_group->deltas[3]->write_symbols[i] = 0;
            delta_group->deltas[4]->write_symbols[i] = 0;
            delta_group->deltas[5]->write_symbols[i] = 0;
            delta_group->deltas[6]->write_symbols[i] = 0;
            delta_group->deltas[7]->write_symbols[i] = 0;
        }
    }
    return delta_group;
}

struct delta_group *end_deltas(struct program *program, int state_add_no_carry, int state_add_carry, int end_state, int term_1_band, int term_2_band, int result_band) {
    struct delta_group *delta_group = malloc(sizeof(struct delta_group));
    delta_group->delta_count = 2;
    delta_group->deltas = malloc(delta_group->delta_count * sizeof(struct delta *));

    char *movement_stop = malloc(program->head_count * sizeof(char ));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == term_1_band || i == term_2_band  || i == result_band)
            movement_stop[i] = '<';
        else
            movement_stop[i] = '-';
    }
    struct delta *end_delta = malloc(sizeof(struct delta));
    end_delta->state_name = state_add_no_carry;
    end_delta->subsequent_state = end_state;
    end_delta->read_symbols = malloc(program->head_count * sizeof(int));
    end_delta->write_symbols = malloc(program->head_count * sizeof(int));
    end_delta->movements = movement_stop;
    for (int i = 0; i < program->head_count; ++i) {
        end_delta->read_symbols[i] = 0;
        end_delta->write_symbols[i] = 0;
    }

    struct delta *overflow_end_delta = malloc(sizeof(struct delta));
    overflow_end_delta->state_name = state_add_carry;
    overflow_end_delta->read_symbols = end_delta->read_symbols;
    overflow_end_delta->write_symbols = end_delta->write_symbols;
    overflow_end_delta->movements = movement_stop;
    overflow_end_delta->subsequent_state = end_state;

    delta_group->deltas[0] = end_delta;
    delta_group->deltas[1] = overflow_end_delta;
    return delta_group;
}

struct delta_group *walk_back_math(struct program *program, int state_name, int subsequent_state, int term_1_band, int term_2_band, int result_band) {
    struct delta_group *delta_group = malloc(sizeof(struct delta_group));
    delta_group->delta_count = 9;
    delta_group->deltas = malloc(delta_group->delta_count * sizeof(struct delta *));

    char *movement_left = malloc(program->head_count * sizeof(char ));
    char *movement_stop = malloc(program->head_count * sizeof(char ));
    int *symbol_blank = malloc(program->head_count * sizeof(int ));
    int *symbol_000 = malloc(program->head_count * sizeof(int ));
    int *symbol_001 = malloc(program->head_count * sizeof(int ));
    int *symbol_010 = malloc(program->head_count * sizeof(int ));
    int *symbol_011 = malloc(program->head_count * sizeof(int ));
    int *symbol_100 = malloc(program->head_count * sizeof(int ));
    int *symbol_101 = malloc(program->head_count * sizeof(int ));
    int *symbol_110 = malloc(program->head_count * sizeof(int ));
    int *symbol_111 = malloc(program->head_count * sizeof(int ));
    for (int i = 0; i < program->head_count; ++i) {
        movement_left[i] = '-';
        movement_stop[i] = '-';
        symbol_blank[i] = 0;
        symbol_000[i] = 0;
        symbol_001[i] = 0;
        symbol_010[i] = 0;
        symbol_011[i] = 0;
        symbol_100[i] = 0;
        symbol_101[i] = 0;
        symbol_110[i] = 0;
        symbol_111[i] = 0;
        if (term_1_band == i) {
            symbol_000[i] = 1;
            symbol_001[i] = 1;
            symbol_010[i] = 1;
            symbol_011[i] = 1;
            symbol_100[i] = 2;
            symbol_101[i] = 2;
            symbol_110[i] = 2;
            symbol_111[i] = 2;
            movement_left[i] = '<';
        }
        if (term_2_band == i) {
            symbol_000[i] = 1;
            symbol_001[i] = 1;
            symbol_010[i] = 2;
            symbol_011[i] = 2;
            symbol_100[i] = 1;
            symbol_101[i] = 1;
            symbol_110[i] = 2;
            symbol_111[i] = 2;
            movement_left[i] = '<';
        }
        if (result_band == i) {
            symbol_000[i] = 1;
            symbol_001[i] = 2;
            symbol_010[i] = 1;
            symbol_011[i] = 2;
            symbol_100[i] = 1;
            symbol_101[i] = 2;
            symbol_110[i] = 1;
            symbol_111[i] = 2;
            movement_left[i] = '<';
        }
    }

    delta_group->deltas[0] = malloc(sizeof(struct delta));
    delta_group->deltas[0]->state_name = state_name;
    delta_group->deltas[0]->read_symbols = symbol_000;
    delta_group->deltas[0]->subsequent_state = state_name;
    delta_group->deltas[0]->write_symbols = symbol_000;
    delta_group->deltas[0]->movements = movement_left;

    delta_group->deltas[1] = malloc(sizeof(struct delta));
    delta_group->deltas[1]->state_name = state_name;
    delta_group->deltas[1]->read_symbols = symbol_001;
    delta_group->deltas[1]->subsequent_state = state_name;
    delta_group->deltas[1]->write_symbols = symbol_001;
    delta_group->deltas[1]->movements = movement_left;

    delta_group->deltas[2] = malloc(sizeof(struct delta));
    delta_group->deltas[2]->state_name = state_name;
    delta_group->deltas[2]->read_symbols = symbol_010;
    delta_group->deltas[2]->subsequent_state = state_name;
    delta_group->deltas[2]->write_symbols = symbol_010;
    delta_group->deltas[2]->movements = movement_left;

    delta_group->deltas[3] = malloc(sizeof(struct delta));
    delta_group->deltas[3]->state_name = state_name;
    delta_group->deltas[3]->read_symbols = symbol_011;
    delta_group->deltas[3]->subsequent_state = state_name;
    delta_group->deltas[3]->write_symbols = symbol_011;
    delta_group->deltas[3]->movements = movement_left;

    delta_group->deltas[4] = malloc(sizeof(struct delta));
    delta_group->deltas[4]->state_name = state_name;
    delta_group->deltas[4]->read_symbols = symbol_100;
    delta_group->deltas[4]->subsequent_state = state_name;
    delta_group->deltas[4]->write_symbols = symbol_100;
    delta_group->deltas[4]->movements = movement_left;

    delta_group->deltas[5] = malloc(sizeof(struct delta));
    delta_group->deltas[5]->state_name = state_name;
    delta_group->deltas[5]->read_symbols = symbol_101;
    delta_group->deltas[5]->subsequent_state = state_name;
    delta_group->deltas[5]->write_symbols = symbol_101;
    delta_group->deltas[5]->movements = movement_left;

    delta_group->deltas[6] = malloc(sizeof(struct delta));
    delta_group->deltas[6]->state_name = state_name;
    delta_group->deltas[6]->read_symbols = symbol_110;
    delta_group->deltas[6]->subsequent_state = state_name;
    delta_group->deltas[6]->write_symbols = symbol_110;
    delta_group->deltas[6]->movements = movement_left;

    delta_group->deltas[7] = malloc(sizeof(struct delta));
    delta_group->deltas[7]->state_name = state_name;
    delta_group->deltas[7]->read_symbols = symbol_111;
    delta_group->deltas[7]->subsequent_state = state_name;
    delta_group->deltas[7]->write_symbols = symbol_111;
    delta_group->deltas[7]->movements = movement_left;

    delta_group->deltas[8] = malloc(sizeof(struct delta));
    delta_group->deltas[8]->state_name = state_name;
    delta_group->deltas[8]->read_symbols = symbol_blank;
    delta_group->deltas[8]->subsequent_state = subsequent_state;
    delta_group->deltas[8]->write_symbols = symbol_blank;
    delta_group->deltas[8]->movements = movement_stop;

    return delta_group;
}

struct delta_group *create_add_deltas(struct program *program, int state_add_no_carry, int state_add_carry, char *movements, int augend_band, int addend_band, int sum_band) {
    // set common attributes
    struct delta_group *delta_group = create_math_deltas(program, movements, augend_band, addend_band, sum_band);
    for (int i = 0; i < delta_group->delta_count; ++i) {
        delta_group->deltas[i]->state_name = state_add_no_carry;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 3 || i == 7)
            delta_group->deltas[i]->subsequent_state = state_add_carry;
        else
            delta_group->deltas[i]->subsequent_state = state_add_no_carry;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == sum_band) {
            delta_group->deltas[0]->write_symbols[i] = 1;
            delta_group->deltas[1]->write_symbols[i] = 2;
            delta_group->deltas[2]->write_symbols[i] = 2;
            delta_group->deltas[3]->write_symbols[i] = 1;
            delta_group->deltas[4]->write_symbols[i] = 1;
            delta_group->deltas[5]->write_symbols[i] = 2;
            delta_group->deltas[6]->write_symbols[i] = 2;
            delta_group->deltas[7]->write_symbols[i] = 1;
        }

    }
    return delta_group;
}

struct delta_group *create_add_deltas_carry_over(struct program *program, int state_add_no_carry, int state_add_carry, char *movements, int augend_band, int addend_band, int sum_band) {
    // set common attributes
    struct delta_group *delta_group = create_math_deltas(program, movements, augend_band, addend_band, sum_band);
    for (int i = 0; i < delta_group->delta_count; ++i) {
        delta_group->deltas[i]->state_name = state_add_carry;
        if (i == 0 || i == 4)
            delta_group->deltas[i]->subsequent_state = state_add_no_carry;
        else
            delta_group->deltas[i]->subsequent_state = state_add_carry;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == sum_band) {
            delta_group->deltas[0]->write_symbols[i] = 2;
            delta_group->deltas[1]->write_symbols[i] = 1;
            delta_group->deltas[2]->write_symbols[i] = 1;
            delta_group->deltas[3]->write_symbols[i] = 2;
            delta_group->deltas[4]->write_symbols[i] = 2;
            delta_group->deltas[5]->write_symbols[i] = 1;
            delta_group->deltas[6]->write_symbols[i] = 1;
            delta_group->deltas[7]->write_symbols[i] = 2;
        }

    }
    return delta_group;
}

struct delta_group *add_operation(struct program *program, int start_state, int subsequent_state, int augend_band, int addend_band, int sum_band) {
    char *movements = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == augend_band || i == addend_band || i == sum_band)
            movements[i] = '>';
        else
            movements[i] = '-';
    }

    program->state_count += 3;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-3] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-3], "%sadde",program->state_names[start_state]);
    int end_add_state = program->state_count - 3;

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%sadd0",program->state_names[start_state]);
    int state_add_no_carry = program->state_count - 2;


    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%sadd1",program->state_names[start_state]);
    int state_add_carry = program->state_count - 1;

    struct delta *start_add_delta = start_delta(program, start_state, state_add_no_carry, movements);
    struct delta_group *add_no_carry_group =  create_add_deltas(program, state_add_no_carry, state_add_carry, movements, augend_band, addend_band, sum_band);
    struct delta_group *add_carry_group = create_add_deltas_carry_over(program, state_add_no_carry, state_add_carry, movements, augend_band, addend_band, sum_band);
    struct delta_group *end_add_group = end_deltas(program, state_add_no_carry, state_add_carry, end_add_state, augend_band, addend_band, sum_band);
    struct delta_group *walk_back_group = walk_back_math(program, end_add_state, subsequent_state, augend_band, addend_band, sum_band);

    struct delta_group *add_deltas = malloc(sizeof(struct delta_group));
    add_deltas->delta_count = 1 + add_no_carry_group->delta_count + add_carry_group->delta_count + end_add_group->delta_count + walk_back_group->delta_count;
    add_deltas->deltas = malloc(add_deltas->delta_count * sizeof(struct delta *));

    int counter = 0;

    add_deltas->deltas[counter++] = start_add_delta;

    for (int i = 0; i < add_no_carry_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = add_no_carry_group->deltas[i];
    }
    free(add_no_carry_group);

    for (int i = 0; i < add_carry_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = add_carry_group->deltas[i];
    }
    free(add_carry_group);

    for (int i = 0; i < end_add_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = end_add_group->deltas[i];
    }
    free(end_add_group);

    for (int i = 0; i < walk_back_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = walk_back_group->deltas[i];
    }
    free(walk_back_group);

    return add_deltas;
}

struct delta_group *create_sub_deltas(struct program *program, int state_sub_no_carry, int state_sub_carry, char *movements, int minuend_band, int subtrahend_band, int difference_band) {
    // set common attributes
    struct delta_group *delta_group = create_math_deltas(program, movements, minuend_band, subtrahend_band, difference_band);
    for (int i = 0; i < delta_group->delta_count; ++i) {
        delta_group->deltas[i]->state_name = state_sub_no_carry;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 1 || i == 5)
            delta_group->deltas[i]->subsequent_state = state_sub_carry;
        else
            delta_group->deltas[i]->subsequent_state = state_sub_no_carry;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == difference_band) {
            delta_group->deltas[0]->write_symbols[i] = 1;
            delta_group->deltas[1]->write_symbols[i] = 2;
            delta_group->deltas[2]->write_symbols[i] = 2;
            delta_group->deltas[3]->write_symbols[i] = 1;
            delta_group->deltas[4]->write_symbols[i] = 1;
            delta_group->deltas[5]->write_symbols[i] = 2;
            delta_group->deltas[6]->write_symbols[i] = 2;
            delta_group->deltas[7]->write_symbols[i] = 1;
        }

    }
    return delta_group;
}

struct delta_group *create_sub_deltas_carry_over(struct program *program, int state_sub_no_carry, int state_sub_carry, char *movements, int minuend_band, int subtrahend_band, int difference_band) {
    // set common attributes
    struct delta_group *delta_group = create_math_deltas(program, movements, minuend_band, subtrahend_band, difference_band);
    for (int i = 0; i < delta_group->delta_count; ++i) {
        delta_group->deltas[i]->state_name = state_sub_carry;
        if (i == 2 || i == 6)
            delta_group->deltas[i]->subsequent_state = state_sub_no_carry;
        else
            delta_group->deltas[i]->subsequent_state = state_sub_carry;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == difference_band) {
            delta_group->deltas[0]->write_symbols[i] = 2;
            delta_group->deltas[1]->write_symbols[i] = 1;
            delta_group->deltas[2]->write_symbols[i] = 1;
            delta_group->deltas[3]->write_symbols[i] = 2;
            delta_group->deltas[4]->write_symbols[i] = 2;
            delta_group->deltas[5]->write_symbols[i] = 1;
            delta_group->deltas[6]->write_symbols[i] = 1;
            delta_group->deltas[7]->write_symbols[i] = 2;
        }

    }
    return delta_group;
}

struct delta_group *sub_operation(struct program *program, int start_state, int subsequent_state, int minuend_band, int subtrahend_band, int difference_band) {
    char *movements = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == minuend_band || i == subtrahend_band || i == difference_band)
            movements[i] = '>';
        else
            movements[i] = '-';
    }
    program->state_count += 3;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-3] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-3], "%ssube",program->state_names[start_state]);
    int end_sub_state = program->state_count - 3;

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%ssub0",program->state_names[start_state]);
    int state_sub_no_carry = program->state_count - 2;


    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%ssub1",program->state_names[start_state]);
    int state_sub_carry = program->state_count - 1;

    struct delta *start_sub_delta = start_delta(program, start_state, state_sub_no_carry, movements);
    struct delta_group *sub_no_carry_group =  create_sub_deltas(program, state_sub_no_carry, state_sub_carry, movements, minuend_band, subtrahend_band, difference_band);
    struct delta_group *sub_carry_group = create_sub_deltas_carry_over(program, state_sub_no_carry, state_sub_carry, movements, minuend_band, subtrahend_band, difference_band);
    struct delta_group *end_sub_group = end_deltas(program, state_sub_no_carry, state_sub_carry, end_sub_state, minuend_band, subtrahend_band, difference_band);
    struct delta_group *walk_back_group = walk_back_math(program, end_sub_state, subsequent_state, minuend_band, subtrahend_band, difference_band);

    struct delta_group *add_deltas = malloc(sizeof(struct delta_group));
    add_deltas->delta_count = 1 + sub_no_carry_group->delta_count + sub_carry_group->delta_count + end_sub_group->delta_count + walk_back_group->delta_count;
    add_deltas->deltas = malloc(add_deltas->delta_count * sizeof(struct delta *));

    int counter = 0;

    add_deltas->deltas[counter++] = start_sub_delta;

    for (int i = 0; i < sub_no_carry_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = sub_no_carry_group->deltas[i];
    }
    free(sub_no_carry_group);

    for (int i = 0; i < sub_carry_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = sub_carry_group->deltas[i];
    }
    free(sub_carry_group);

    for (int i = 0; i < end_sub_group->delta_count; ++i) {
        add_deltas->deltas[counter++] = end_sub_group->deltas[i];
    }
    free(end_sub_group);

    for (int i = 0; i < walk_back_group->delta_count; ++i) {
        struct delta *d = walk_back_group->deltas[i];
        add_deltas->deltas[counter++] = walk_back_group->deltas[i];
    }
    free(walk_back_group);

    return add_deltas;
}

struct delta_group *create_write_number_deltas(struct program *program, int state_name, int subsequent_state, int band, int number) {
    char *movement_right = malloc(program->head_count * sizeof(char ));
    char *movement_left = malloc(program->head_count * sizeof(char ));
    char *movement_stop = malloc(program->head_count * sizeof(char ));
    int *symbol_blank = malloc(program->head_count * sizeof(int ));
    int *symbol_0 = malloc(program->head_count * sizeof(int ));
    int *symbol_1 = malloc(program->head_count * sizeof(int ));
    for (int i = 0; i < program->head_count; ++i) {
        if (band == i) {
            movement_right[i] = '>';
            movement_left[i] = '<';
            symbol_blank[i] = 0;
            symbol_0[i] = 1;
            symbol_1[i] = 2;
            continue;
        }
        movement_right[i] = '-';
        movement_left[i] = '-';
        movement_stop[i] = '-';
        symbol_blank[i] = 0;
        symbol_0[i] = 0;
        symbol_1[i] = 0;
    }

    struct delta_group *write_deltas = malloc(sizeof(struct delta_group));
    int int_size = sizeof(int) * 8;
    int str_len = strlen(program->state_names[state_name]);
    write_deltas->delta_count = (2 * int_size);
    write_deltas->deltas = malloc(write_deltas->delta_count * sizeof(struct delta *));

    char *number_str = calloc(int_size + 1, sizeof(char ));
    int_to_bin(number, number_str, int_size);

    program->state_names = realloc(program->state_names, (program->state_count + int_size + 1) * sizeof(char *));

    program->state_names[program->state_count] = calloc(str_len + 2, sizeof(char));
    sprintf(program->state_names[program->state_count], "%s0", program->state_names[state_name]);
    program->state_count++;

    write_deltas->deltas[0] = malloc(sizeof(struct delta));
    write_deltas->deltas[0]->state_name = state_name;
    write_deltas->deltas[0]->read_symbols = symbol_blank;
    write_deltas->deltas[0]->subsequent_state = program->state_count - 1;
    write_deltas->deltas[0]->write_symbols = symbol_blank;
    write_deltas->deltas[0]->movements = movement_right;

    for (int i = 0; i < int_size - 1; ++i) {
        program->state_names[program->state_count] = calloc(str_len + int_length(i + 1) + 1, sizeof(char));
        sprintf(program->state_names[program->state_count], "%s%d", program->state_names[state_name], i + 1);
        program->state_count++;

        write_deltas->deltas[i + 1] = malloc(sizeof(struct delta));
        write_deltas->deltas[i + 1]->state_name = program->state_count - 2;
        write_deltas->deltas[i + 1]->read_symbols = symbol_0;
        write_deltas->deltas[i + 1]->subsequent_state = program->state_count - 1;
        if (number_str[i] == '0')
            write_deltas->deltas[i + 1]->write_symbols = symbol_0;
        else
            write_deltas->deltas[i + 1]->write_symbols = symbol_1;
        write_deltas->deltas[i + 1]->movements = movement_right;

        write_deltas->deltas[i + int_size] = malloc(sizeof(struct delta));
        write_deltas->deltas[i + int_size]->state_name = program->state_count - 2;
        write_deltas->deltas[i + int_size]->read_symbols = symbol_1;
        write_deltas->deltas[i + int_size]->subsequent_state = program->state_count - 1;
        if (number_str[i] == '0')
            write_deltas->deltas[i + int_size]->write_symbols = symbol_0;
        else
            write_deltas->deltas[i + int_size]->write_symbols = symbol_1;
        write_deltas->deltas[i + int_size]->movements = movement_right;
    }
    write_deltas->deltas[(2 * int_size) - 1] = malloc(sizeof(struct delta));
    write_deltas->deltas[(2 * int_size) - 1]->state_name = program->state_count - 1;
    write_deltas->deltas[(2 * int_size) - 1]->read_symbols = symbol_blank;
    write_deltas->deltas[(2 * int_size) - 1]->subsequent_state = subsequent_state;
    write_deltas->deltas[(2 * int_size) - 1]->write_symbols = symbol_blank;
    write_deltas->deltas[(2 * int_size) - 1]->movements = movement_left;

    return write_deltas;
}

struct delta_group *walk_back_one(struct program *program, int state_name, int subsequent_state, int band) {
    struct delta_group *delta_group = malloc(sizeof(struct delta_group));
    delta_group->delta_count = 3;
    delta_group->deltas = malloc(delta_group->delta_count * sizeof(struct delta));

    char *movements_left = malloc(program->head_count * sizeof(char));
    char *movements_stop = malloc(program->head_count * sizeof(char));
    int * symbol_0 = malloc(program->head_count * sizeof(char));
    int * symbol_1 = malloc(program->head_count * sizeof(char));
    int * symbol_blank = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == band) {
            symbol_0[i] = 1;
            symbol_1[i] = 2;
            movements_left[i] = '<';
        } else {
            symbol_0[i] = 0;
            symbol_1[i] = 0;
            movements_left[i] = '-';
        }
        movements_stop[i] = '-';
        symbol_blank[i] = 0;
    }

    delta_group->deltas[0] = malloc(sizeof(struct delta));
    delta_group->deltas[0]->state_name = state_name;
    delta_group->deltas[0]->read_symbols = symbol_0;
    delta_group->deltas[0]->subsequent_state = state_name;
    delta_group->deltas[0]->write_symbols = symbol_0;
    delta_group->deltas[0]->movements = movements_left;

    delta_group->deltas[1] = malloc(sizeof(struct delta));
    delta_group->deltas[1]->state_name = state_name;
    delta_group->deltas[1]->read_symbols = symbol_1;
    delta_group->deltas[1]->subsequent_state = state_name;
    delta_group->deltas[1]->write_symbols = symbol_1;
    delta_group->deltas[1]->movements = movements_left;

    delta_group->deltas[2] = malloc(sizeof(struct delta));
    delta_group->deltas[2]->state_name = state_name;
    delta_group->deltas[2]->read_symbols = symbol_blank;
    delta_group->deltas[2]->subsequent_state = subsequent_state;
    delta_group->deltas[2]->write_symbols = symbol_blank;
    delta_group->deltas[2]->movements = movements_stop;

    return delta_group;
}

struct delta_group *write_number_to_tape(struct program *program, int start_state, int subsequent_state, int band, int number) {
    char *movements = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == band)
            movements[i] = '>';
        else
            movements[i] = '-';
    }
    program->state_count += 2;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%since",program->state_names[start_state]);
    int end_add_state = program->state_count - 2;

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[start_state]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%sincr",program->state_names[start_state]);
    int state_incr_start = program->state_count - 1;

    //struct delta *start_incr_delta = start_delta(program, start_state, state_incr_start, movements);
    struct delta_group *write_deltas = create_write_number_deltas(program, start_state, end_add_state, band, number);
    struct delta_group *walk_back_deltas = walk_back_one(program, end_add_state, subsequent_state, band);

    struct delta_group *incr_delta_group = malloc(sizeof(struct delta_group));
    incr_delta_group->delta_count = write_deltas->delta_count + walk_back_deltas->delta_count;
    incr_delta_group->deltas = malloc(incr_delta_group->delta_count * sizeof(struct delta*));

    int counter = 0;
    //incr_delta_group->deltas[counter++] = start_incr_delta;

    for (int i = 0; i < write_deltas->delta_count; ++i) {
        incr_delta_group->deltas[counter++] = write_deltas->deltas[i];
    }
    free(write_deltas);

    for (int i = 0; i < walk_back_deltas->delta_count; ++i) {
        incr_delta_group->deltas[counter++] = walk_back_deltas->deltas[i];
    }
    free(walk_back_deltas);

    return incr_delta_group;
}

void create_incr_states(struct program *program, int base_state_name) {
    program->state_names = realloc(program->state_names, (program->state_count + 5) * sizeof(char *) );

    program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count++], "%swadd", program->state_names[base_state_name]);
}

struct delta_group *incr_operation(struct program *program, int start_state, int subsequent_state, int incr_band) {
    create_incr_states(program, start_state);
    int end_write_state = program->state_count - 1;
    struct delta_group *write_deltas = write_number_to_tape(program, start_state, end_write_state,
                                                            program->head_count - 1, 1);
    struct delta_group *add_deltas = add_operation(program, end_write_state, subsequent_state, incr_band, program->head_count - 1, incr_band);

    struct delta_group *incr_deltas = malloc(sizeof(struct delta_group));
    incr_deltas->delta_count = write_deltas->delta_count + add_deltas->delta_count;
    incr_deltas->deltas = malloc(incr_deltas->delta_count * sizeof(struct delta *));

    for (int i = 0; i < write_deltas->delta_count; ++i) {
        incr_deltas->deltas[i] = write_deltas->deltas[i];
    }

    for (int i = 0; i < add_deltas->delta_count; ++i) {
        incr_deltas->deltas[write_deltas->delta_count + i] = add_deltas->deltas[i];
    }

    free(write_deltas);
    free(add_deltas);

    return incr_deltas;
}

void create_decr_states(struct program *program, int base_state_name) {
    program->state_names = realloc(program->state_names, (program->state_count + 5) * sizeof(char *) );

    program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count++], "%swsub", program->state_names[base_state_name]);
}

struct delta_group *decr_operation(struct program *program, int start_state, int subsequent_state, int decr_band) {
    create_decr_states(program, start_state);
    int end_write_state = program->state_count - 1;
    struct delta_group *write_deltas = write_number_to_tape(program, start_state, end_write_state,
                                                            program->head_count - 1, 1);
    struct delta_group *sub_deltas = sub_operation(program, end_write_state, subsequent_state, decr_band, program->head_count - 1, decr_band);

    struct delta_group *decr_deltas = malloc(sizeof(struct delta_group));
    decr_deltas->delta_count = write_deltas->delta_count + sub_deltas->delta_count;
    decr_deltas->deltas = malloc(decr_deltas->delta_count * sizeof(struct delta *));

    for (int i = 0; i < write_deltas->delta_count; ++i) {
        decr_deltas->deltas[i] = write_deltas->deltas[i];
    }

    for (int i = 0; i < sub_deltas->delta_count; ++i) {
        decr_deltas->deltas[write_deltas->delta_count + i] = sub_deltas->deltas[i];
    }

    free(write_deltas);
    free(sub_deltas);

    return decr_deltas;
}