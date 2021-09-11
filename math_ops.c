#include <stdlib.h>
#include <string.h>
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

int end_deltas(struct program *program, struct delta **delta_group, char *movements) {
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
    overflow_end_delta->subsequent_state = program->state_count - 1;

    delta_group[0] = end_delta;
    delta_group[1] = overflow_end_delta;
    return 2;
}

void create_add_states(struct program *program, int base_state_name) {
    program->state_count += 3;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-3] = calloc(strlen(program->state_names[base_state_name]) + 4, sizeof(char));
    strcat(program->state_names[program->state_count-3],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-3], "add");

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    strcat(program->state_names[program->state_count-2],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-2], "add1");

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[base_state_name]) + 7, sizeof(char));
    strcat(program->state_names[program->state_count-1],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-1], "addend");
}

int create_add_deltas(struct program *program, struct delta **delta_group, char *movements, int augend_band, int addend_band, int sum_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, augend_band, addend_band, sum_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 3;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 3 || i == 7)
            delta_group[i]->subsequent_state = program->state_count - 2;
        else
            delta_group[i]->subsequent_state = program->state_count - 3;
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
        delta_group[i]->state_name = program->state_count - 2;
        if (i == 0 || i == 4)
            delta_group[i]->subsequent_state = program->state_count - 3;
        else
            delta_group[i]->subsequent_state = program->state_count - 2;
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

struct delta_group *add_operation(struct program *program, int start_state, int augend_band, int addend_band, int sum_band) {
    create_add_states(program, start_state);
    struct delta_group *add_deltas = malloc(sizeof(struct delta_group));
    add_deltas->delta_count = 19;
    add_deltas->deltas = malloc(add_deltas->delta_count * sizeof(struct delta *));
    add_deltas->start_state = start_state;
    add_deltas->subsequent_state = program->state_count - 1;

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
    counter += end_deltas(program, add_deltas->deltas + counter, movements);
    return add_deltas;
}

struct delta_group *mult_operation(struct program *program, int start_state, int multiplier_band, int multiplicand_band, int product_band) {
    struct delta_group *mult_deltas = malloc(sizeof(struct delta_group));

    return mult_deltas;
}

void create_sub_states(struct program *program, int base_state_name) {
    program->state_count += 3;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-3] = calloc(strlen(program->state_names[base_state_name]) + 4, sizeof(char));
    strcat(program->state_names[program->state_count-3],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-3], "sub");

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    strcat(program->state_names[program->state_count-2],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-2], "sub1");

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[base_state_name]) + 7, sizeof(char));
    strcat(program->state_names[program->state_count-1],program->state_names[base_state_name]);
    strcat(program->state_names[program->state_count-1], "subend");
}

int create_sub_deltas(struct program *program, struct delta **delta_group, char *movements, int minuend_band, int subtrahend_band, int difference_band) {
    // set common attributes
    int delta_count = create_math_deltas(program, delta_group, movements, minuend_band, subtrahend_band, difference_band);
    for (int i = 0; i < delta_count; ++i) {
        delta_group[i]->state_name = program->state_count - 3;
        // only deltas which result in a carry over should have the corresponding subsequent state
        if (i == 1 || i == 5)
            delta_group[i]->subsequent_state = program->state_count - 2;
        else
            delta_group[i]->subsequent_state = program->state_count - 3;
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
        delta_group[i]->state_name = program->state_count - 2;
        if (i == 3 || i == 7)
            delta_group[i]->subsequent_state = program->state_count - 3;
        else
            delta_group[i]->subsequent_state = program->state_count - 2;
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

struct delta_group *sub_operation(struct program *program, int start_state, int minuend_band, int subtrahend_band, int difference_band) {
    create_sub_states(program, start_state);
    struct delta_group *sub_deltas = malloc(sizeof(struct delta_group));
    sub_deltas->delta_count = 19;
    sub_deltas->deltas = malloc(sub_deltas->delta_count * sizeof(struct delta *));
    sub_deltas->start_state = start_state;
    sub_deltas->subsequent_state = program->state_count - 1;

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
    counter += end_deltas(program, sub_deltas->deltas + counter, movements);
    return sub_deltas;
}

struct delta_group *incr_operation(struct program *program, int start_state, int incr_band) {
    struct delta_group *incr_deltas = malloc(sizeof(struct delta_group));

    return incr_deltas;
}