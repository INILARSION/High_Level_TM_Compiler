#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helper.h"

struct delta *start_delta(struct program *program, int state_name, char *movements) {
    struct delta *delta = malloc(sizeof(struct delta));
    delta->state_name = state_name;
    // For the add operation 3 new states were introduced. The first of them is the next state
    delta->subsequent_state = program->state_count - 3;
    delta->read_symbols = malloc(program->head_count * sizeof(int));
    delta->write_symbols = malloc(program->head_count * sizeof(int));
    delta->movements = movements;
    for (int i = 0; i < program->head_count; ++i) {
        delta->read_symbols[i] = 0;
        delta->write_symbols[i] = 0;
    }
    return delta;
}

int create_math_deltas(struct program *program, struct delta **delta_group, char *movements, int term_1_band, int term_2_band, int result_band) {
    // set common attributes
    int delta_count = 8;
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i] = malloc(sizeof(struct delta *));
        delta_group[i]->read_symbols = calloc(program->head_count, sizeof(int));
        delta_group[i]->write_symbols = calloc(program->head_count, sizeof(int));
        delta_group[i]->movements = movements;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == result_band) {
            delta_group[0]->read_symbols[i] = 1;
            delta_group[1]->read_symbols[i] = 1;
            delta_group[2]->read_symbols[i] = 1;
            delta_group[3]->read_symbols[i] = 1;
            delta_group[4]->read_symbols[i] = 2;
            delta_group[5]->read_symbols[i] = 2;
            delta_group[6]->read_symbols[i] = 2;
            delta_group[7]->read_symbols[i] = 2;
        }
        if (i == term_1_band) {
            delta_group[0]->read_symbols[i] = 1;
            delta_group[1]->read_symbols[i] = 1;
            delta_group[2]->read_symbols[i] = 2;
            delta_group[3]->read_symbols[i] = 2;
            delta_group[4]->read_symbols[i] = 1;
            delta_group[5]->read_symbols[i] = 1;
            delta_group[6]->read_symbols[i] = 2;
            delta_group[7]->read_symbols[i] = 2;

            // write the same symbol on the tape as was written
            delta_group[0]->write_symbols[i] = 1;
            delta_group[1]->write_symbols[i] = 1;
            delta_group[2]->write_symbols[i] = 2;
            delta_group[3]->write_symbols[i] = 2;
            delta_group[4]->write_symbols[i] = 1;
            delta_group[5]->write_symbols[i] = 1;
            delta_group[6]->write_symbols[i] = 2;
            delta_group[7]->write_symbols[i] = 2;
        } else if (i == term_2_band) {
            delta_group[0]->read_symbols[i] = 1;
            delta_group[1]->read_symbols[i] = 2;
            delta_group[2]->read_symbols[i] = 1;
            delta_group[3]->read_symbols[i] = 2;
            delta_group[4]->read_symbols[i] = 1;
            delta_group[5]->read_symbols[i] = 2;
            delta_group[6]->read_symbols[i] = 1;
            delta_group[7]->read_symbols[i] = 2;

            delta_group[0]->write_symbols[i] = 1;
            delta_group[1]->write_symbols[i] = 2;
            delta_group[2]->write_symbols[i] = 1;
            delta_group[3]->write_symbols[i] = 2;
            delta_group[4]->write_symbols[i] = 1;
            delta_group[5]->write_symbols[i] = 2;
            delta_group[6]->write_symbols[i] = 1;
            delta_group[7]->write_symbols[i] = 2;
        } else {
            // every other tape than the terms and result bands should read and write a blank
            delta_group[0]->read_symbols[i] = 0;
            delta_group[1]->read_symbols[i] = 0;
            delta_group[2]->read_symbols[i] = 0;
            delta_group[3]->read_symbols[i] = 0;
            delta_group[4]->read_symbols[i] = 0;
            delta_group[5]->read_symbols[i] = 0;
            delta_group[6]->read_symbols[i] = 0;
            delta_group[7]->read_symbols[i] = 0;

            delta_group[0]->write_symbols[i] = 0;
            delta_group[1]->write_symbols[i] = 0;
            delta_group[2]->write_symbols[i] = 0;
            delta_group[3]->write_symbols[i] = 0;
            delta_group[4]->write_symbols[i] = 0;
            delta_group[5]->write_symbols[i] = 0;
            delta_group[6]->write_symbols[i] = 0;
            delta_group[7]->write_symbols[i] = 0;
        }
    }
    return delta_count;
}

int end_deltas(struct program *program, struct delta **delta_group, char *movements, int subsequent_state) {
    struct delta *end_delta = malloc(sizeof(struct delta));
    end_delta->state_name = program->state_count - 3;
    // For the add operation 3 new states were introduced. The last of them is the next state
    end_delta->subsequent_state = program->state_count - 1;
    end_delta->read_symbols = malloc(program->head_count * sizeof(int));
    end_delta->write_symbols = malloc(program->head_count * sizeof(int));
    end_delta->movements = movements;
    for (int i = 0; i < program->head_count; ++i) {
        end_delta->read_symbols[i] = 0;
        end_delta->write_symbols[i] = 0;
    }

    struct delta *overflow_end_delta = malloc(sizeof(struct delta));
    overflow_end_delta->state_name = program->state_count - 2;
    overflow_end_delta->read_symbols = end_delta->read_symbols;
    overflow_end_delta->write_symbols = end_delta->write_symbols;
    overflow_end_delta->movements = movements;
    overflow_end_delta->subsequent_state = subsequent_state;

    delta_group[0] = end_delta;
    delta_group[1] = overflow_end_delta;
    return 2;
}

void create_add_states(struct program *program, int base_state_name) {
    program->state_count += 2;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[base_state_name]) + 4, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%sadd",program->state_names[base_state_name]);

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%sadd1",program->state_names[base_state_name]);
}

int create_add_deltas(struct program *program, struct delta **delta_group, char *movements, int augend_band, int addend_band, int sum_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, augend_band, addend_band, sum_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 2;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 3 || i == 7)
            delta_group[i]->subsequent_state = program->state_count - 1;
        else
            delta_group[i]->subsequent_state = program->state_count - 2;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == sum_band) {
            delta_group[0]->write_symbols[i] = 1;
            delta_group[1]->write_symbols[i] = 2;
            delta_group[2]->write_symbols[i] = 2;
            delta_group[3]->write_symbols[i] = 1;
            delta_group[4]->write_symbols[i] = 1;
            delta_group[5]->write_symbols[i] = 2;
            delta_group[6]->write_symbols[i] = 2;
            delta_group[7]->write_symbols[i] = 1;
        }

    }
    return delta_count;
}

int create_add_deltas_carry_over(struct program *program, struct delta **delta_group, char *movements, int augend_band, int addend_band, int sum_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, augend_band, addend_band, sum_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 1;
        if (i == 0 || i == 4)
            delta_group[i]->subsequent_state = program->state_count - 2;
        else
            delta_group[i]->subsequent_state = program->state_count - 1;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == sum_band) {
            delta_group[0]->write_symbols[i] = 2;
            delta_group[1]->write_symbols[i] = 1;
            delta_group[2]->write_symbols[i] = 1;
            delta_group[3]->write_symbols[i] = 2;
            delta_group[4]->write_symbols[i] = 2;
            delta_group[5]->write_symbols[i] = 1;
            delta_group[6]->write_symbols[i] = 1;
            delta_group[7]->write_symbols[i] = 2;
        }

    }
    return delta_count;
}

struct delta_group *add_operation(struct program *program, int start_state, int subsequent_state, int augend_band, int addend_band, int sum_band) {
    create_add_states(program, start_state);
    struct delta_group *add_deltas = malloc(sizeof(struct delta_group));
    add_deltas->delta_count = 19;
    add_deltas->deltas = malloc(add_deltas->delta_count * sizeof(struct delta *));
    add_deltas->start_state = start_state;
    add_deltas->subsequent_state = subsequent_state;

    char *movements = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == augend_band || i == addend_band || i == sum_band)
            movements[i] = '>';
        else
            movements[i] = '-';
    }
    int counter = 0;
    add_deltas->deltas[counter++] = start_delta(program, start_state, movements);
    counter += create_add_deltas(program, add_deltas->deltas + counter, movements, augend_band, addend_band, sum_band);
    counter += create_add_deltas_carry_over(program, add_deltas->deltas + counter, movements, augend_band, addend_band, sum_band);
    counter += end_deltas(program, add_deltas->deltas + counter, movements, subsequent_state);
    return add_deltas;
}

void create_sub_states(struct program *program, int base_state_name) {
    program->state_count += 2;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[base_state_name]) + 4, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%ssub",program->state_names[base_state_name]);

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%ssub1",program->state_names[base_state_name]);
}

int create_sub_deltas(struct program *program, struct delta **delta_group, char *movements, int minuend_band, int subtrahend_band, int difference_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, minuend_band, subtrahend_band, difference_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 2;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 1 || i == 5)
            delta_group[i]->subsequent_state = program->state_count - 1;
        else
            delta_group[i]->subsequent_state = program->state_count - 2;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == difference_band) {
            delta_group[0]->write_symbols[i] = 1;
            delta_group[1]->write_symbols[i] = 2;
            delta_group[2]->write_symbols[i] = 2;
            delta_group[3]->write_symbols[i] = 1;
            delta_group[4]->write_symbols[i] = 1;
            delta_group[5]->write_symbols[i] = 2;
            delta_group[6]->write_symbols[i] = 2;
            delta_group[7]->write_symbols[i] = 1;
        }

    }
    return delta_count;
}

int create_sub_deltas_carry_over(struct program *program, struct delta **delta_group, char *movements, int minuend_band, int subtrahend_band, int difference_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, minuend_band, subtrahend_band, difference_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 1;
        if (i == 3 || i == 7)
            delta_group[i]->subsequent_state = program->state_count - 2;
        else
            delta_group[i]->subsequent_state = program->state_count - 1;
    }

    // set read/write symbols for the deltas
    for (int i = 0; i < program->head_count; ++i) {
        if (i == difference_band) {
            delta_group[0]->write_symbols[i] = 2;
            delta_group[1]->write_symbols[i] = 1;
            delta_group[2]->write_symbols[i] = 2;
            delta_group[3]->write_symbols[i] = 1;
            delta_group[4]->write_symbols[i] = 2;
            delta_group[5]->write_symbols[i] = 1;
            delta_group[6]->write_symbols[i] = 2;
            delta_group[7]->write_symbols[i] = 1;
        }

    }
    return delta_count;
}

struct delta_group *sub_operation(struct program *program, int start_state, int subsequent_state, int minuend_band, int subtrahend_band, int difference_band) {
    create_sub_states(program, start_state);
    struct delta_group *sub_deltas = malloc(sizeof(struct delta_group));
    sub_deltas->delta_count = 19;
    sub_deltas->deltas = malloc(sub_deltas->delta_count * sizeof(struct delta *));
    sub_deltas->start_state = start_state;
    sub_deltas->subsequent_state = subsequent_state;

    char *movements = malloc(program->head_count * sizeof(char));
    for (int i = 0; i < program->head_count; ++i) {
        if (i == minuend_band || i == subtrahend_band || i == difference_band)
            movements[i] = '>';
        else
            movements[i] = '-';
    }

    int counter = 0;
    sub_deltas->deltas[counter++] = start_delta(program, start_state, movements);
    counter += create_sub_deltas(program, sub_deltas->deltas + counter, movements, minuend_band, subtrahend_band, difference_band);
    counter += create_sub_deltas_carry_over(program, sub_deltas->deltas + counter, movements, minuend_band, subtrahend_band, difference_band);
    counter += end_deltas(program, sub_deltas->deltas + counter, movements, subsequent_state);
    return sub_deltas;
}

void create_number_states(struct program *program, int base_state_name, int num_states) {
    program->state_names = realloc(program->state_names, (program->state_count +  num_states + 2) * sizeof(char *) );

    // state for going back
    program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count++], "%snumsb", program->state_names[base_state_name]);

    // state for going back to beginning
    program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count++], "%snumb", program->state_names[base_state_name]);

    // states for writing all bits of the number
    for (int i = 0; i < num_states; ++i) {
        program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 8, sizeof(char));
        sprintf(program->state_names[program->state_count++], "%snum%d", program->state_names[base_state_name], i);
    }
}

struct delta_group * write_number_to_tape(struct program *program, int start_state, int subsequent_state, int band, int number) {
    struct delta_group *write_deltas = malloc(sizeof(struct delta_group));
    int int_size = (sizeof(int) * 8);
    write_deltas->delta_count = (2 * int_size) + 6;
    write_deltas->deltas = malloc(write_deltas->delta_count * sizeof(struct delta *));
    write_deltas->start_state = start_state;
    write_deltas->subsequent_state = subsequent_state;

    struct delta **delta_group = write_deltas->deltas;

    char *number_str = calloc(int_size + 1, sizeof(char ));
    int_to_bin(number, number_str, int_size);

    create_number_states(program, start_state, int_size);

    delta_group[0] = malloc(sizeof(struct delta));
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

    // start writing
    delta_group[0]->state_name = start_state;
    delta_group[0]->read_symbols = symbol_blank;
    delta_group[0]->write_symbols = symbol_blank;
    delta_group[0]->movements = movement_right;
    delta_group[0]->subsequent_state = program->state_count - int_size;

    // write numbers
    for (int i = 0; i < int_size; ++i) {
        delta_group[1 + i]->state_name = program->state_count - (int_size - i);
        delta_group[1 + i]->read_symbols = symbol_0;
        if (number_str[i] == '0')
            delta_group[1 + i]->write_symbols = symbol_0;
        else
            delta_group[1 + i]->write_symbols = symbol_1;
        delta_group[1 + i]->movements = movement_right;
        if (i < int_size - 1)
            delta_group[1 + i]->subsequent_state = program->state_count - (int_size - (i + 1));
        else
            delta_group[1 + i]->subsequent_state = program->state_count - int_size - 2;

        delta_group[1 + int_size + i]->state_name = program->state_count - (int_size - i);
        delta_group[1 + int_size + i]->read_symbols = symbol_1;
        if (number_str[i] == '0')
            delta_group[1 + int_size + i]->write_symbols = symbol_0;
        else
            delta_group[1 + int_size + i]->write_symbols = symbol_1;
        delta_group[1 + int_size + i]->movements = movement_right;
        if (i < int_size - 1)
            delta_group[1 + i]->subsequent_state = program->state_count - (int_size - (i + 1));
        else
            delta_group[1 + i]->subsequent_state = program->state_count - int_size - 2;
    }

    // stop writing
    delta_group[(2 * int_size) + 2]->state_name = program->state_count - int_size - 2;
    delta_group[(2 * int_size) + 2]->read_symbols = symbol_blank;
    delta_group[(2 * int_size) + 2]->write_symbols = symbol_blank;
    delta_group[(2 * int_size) + 2]->movements = movement_left;
    delta_group[(2 * int_size) + 2]->subsequent_state = program->state_count - int_size - 1;

    // go back
    delta_group[(2 * int_size) + 3]->state_name = program->state_count - int_size - 1;
    delta_group[(2 * int_size) + 3]->read_symbols = symbol_0;
    delta_group[(2 * int_size) + 3]->write_symbols = symbol_0;
    delta_group[(2 * int_size) + 3]->movements = movement_left;
    delta_group[(2 * int_size) + 3]->subsequent_state = program->state_count - int_size - 1;

    delta_group[(2 * int_size) + 4]->state_name = program->state_count - int_size - 1;
    delta_group[(2 * int_size) + 4]->read_symbols = symbol_1;
    delta_group[(2 * int_size) + 4]->write_symbols = symbol_1;
    delta_group[(2 * int_size) + 4]->movements = movement_left;
    delta_group[(2 * int_size) + 4]->subsequent_state = program->state_count - int_size - 1;

    // stop going back
    delta_group[(2 * int_size) + 5]->state_name = program->state_count - int_size - 1;
    delta_group[(2 * int_size) + 5]->read_symbols = symbol_blank;
    delta_group[(2 * int_size) + 5]->write_symbols = symbol_blank;
    delta_group[(2 * int_size) + 5]->movements = movement_stop;
    delta_group[(2 * int_size) + 5]->subsequent_state = subsequent_state;
    return write_deltas;
}

void create_incr_states(struct program *program, int base_state_name) {
    program->state_names = realloc(program->state_names, (program->state_count + 5) * sizeof(char *) );

    program->state_names[program->state_count] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count++], "%swadd", program->state_names[base_state_name]);
}

struct delta_group *incr_operation(struct program *program, int start_state, int subsequent_state, int incr_band) {
    create_incr_states(program, start_state);


    struct delta_group *write_deltas = write_number_to_tape(program, start_state, program->state_count - 1, program->head_count - 1, 1);
    struct delta_group *add_deltas = add_operation(program, program->state_count - 1, subsequent_state, incr_band, program->head_count - 1, incr_band);

    struct delta_group *incr_deltas = malloc(sizeof(struct delta_group));
    incr_deltas->delta_count = write_deltas->delta_count + add_deltas->delta_count;
    incr_deltas->deltas = malloc(incr_deltas->delta_count * sizeof(struct delta *));
    incr_deltas->start_state = start_state;
    incr_deltas->subsequent_state = subsequent_state;

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

struct delta_group *decr_operation(struct program *program, int start_state, int subsequent_state, int incr_band) {
    create_decr_states(program, start_state);


    struct delta_group *write_deltas = write_number_to_tape(program, start_state, program->state_count - 1, program->head_count - 1, 1);
    struct delta_group *sub_deltas = sub_operation(program, program->state_count - 1, subsequent_state, incr_band, program->head_count - 1, incr_band);

    struct delta_group *decr_deltas = malloc(sizeof(struct delta_group));
    decr_deltas->delta_count = write_deltas->delta_count + sub_deltas->delta_count;
    decr_deltas->deltas = malloc(decr_deltas->delta_count * sizeof(struct delta *));
    decr_deltas->start_state = start_state;
    decr_deltas->subsequent_state = subsequent_state;

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