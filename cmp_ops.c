#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmp_ops.h"
#include "helper.h"

void create_equal_states(struct program *program, int base_state_name) {
    program->state_count += 3;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));

    program->state_names[program->state_count-3] = calloc(strlen(program->state_names[base_state_name]) + 5, sizeof(char));
    sprintf(program->state_names[program->state_count-3], "%sequf",program->state_names[base_state_name]);

    program->state_names[program->state_count-2] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count-2], "%sequb1",program->state_names[base_state_name]);

    program->state_names[program->state_count-1] = calloc(strlen(program->state_names[base_state_name]) + 6, sizeof(char));
    sprintf(program->state_names[program->state_count-1], "%sequb2",program->state_names[base_state_name]);
}

struct delta_group *equal_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2) {
    create_equal_states(program, start_state);
    struct delta_group *equal_deltas = malloc(sizeof(struct delta_group));
    equal_deltas->delta_count = 13;
    equal_deltas->deltas = malloc(equal_deltas->delta_count * sizeof(struct delta *));
    equal_deltas->start_state = start_state;
    equal_deltas->subsequent_state = subsequent_state_true;

    // setup all read/write symbols and movements
    char *movement_right = malloc(program->head_count * sizeof(char ));
    char *movement_left = malloc(program->head_count * sizeof(char ));
    char *movement_stop = malloc(program->head_count * sizeof(char ));
    int *symbol_blank = malloc(program->head_count * sizeof(int ));
    int *symbol_0 = malloc(program->head_count * sizeof(int ));
    int *symbol_01 = malloc(program->head_count * sizeof(int ));
    int *symbol_1 = malloc(program->head_count * sizeof(int ));
    int *symbol_10 = malloc(program->head_count * sizeof(int ));
    for (int i = 0; i < program->head_count; ++i) {
        if (band_1 == i) {
            movement_right[i] = '>';
            movement_left[i] = '<';
            symbol_blank[i] = 0;
            symbol_0[i] = 1;
            symbol_01[i] = 1;
            symbol_1[i] = 2;
            symbol_10[i] = 2;
            continue;
        } else if (band_2 == i) {
            movement_right[i] = '>';
            movement_left[i] = '<';
            symbol_blank[i] = 0;
            symbol_0[i] = 1;
            symbol_01[i] = 2;
            symbol_1[i] = 2;
            symbol_10[i] = 1;
            continue;
        }
        movement_right[i] = '-';
        movement_left[i] = '-';
        movement_stop[i] = '-';
        symbol_blank[i] = 0;
        symbol_0[i] = 0;
        symbol_01[i] = 0;
        symbol_1[i] = 0;
        symbol_10[i] = 0;
    }

    // start compare
    equal_deltas->deltas[0] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[0]->state_name = start_state;
    equal_deltas->deltas[0]->read_symbols = symbol_blank;
    equal_deltas->deltas[0]->subsequent_state = program->state_count - 3;
    equal_deltas->deltas[0]->write_symbols = symbol_blank;
    equal_deltas->deltas[0]->movements = movement_right;

    // both 0
    equal_deltas->deltas[1] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[1]->state_name = program->state_count - 3;
    equal_deltas->deltas[1]->read_symbols = symbol_0;
    equal_deltas->deltas[1]->subsequent_state = program->state_count - 3;
    equal_deltas->deltas[1]->write_symbols = symbol_0;
    equal_deltas->deltas[1]->movements = movement_right;

    // both 1
    equal_deltas->deltas[2] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[2]->state_name = program->state_count - 3;
    equal_deltas->deltas[2]->read_symbols = symbol_1;
    equal_deltas->deltas[2]->subsequent_state = program->state_count - 3;
    equal_deltas->deltas[2]->write_symbols = symbol_1;
    equal_deltas->deltas[2]->movements = movement_right;

    // all were equal and blank is read, go back
    equal_deltas->deltas[3] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[3]->state_name = program->state_count - 3;
    equal_deltas->deltas[3]->read_symbols = symbol_blank;
    equal_deltas->deltas[3]->subsequent_state = program->state_count - 2;
    equal_deltas->deltas[3]->write_symbols = symbol_blank;
    equal_deltas->deltas[3]->movements = movement_left;

    // bits were different (1,0)
    equal_deltas->deltas[4] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[4]->state_name = program->state_count - 3;
    equal_deltas->deltas[4]->read_symbols = symbol_10;
    equal_deltas->deltas[4]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[4]->write_symbols = symbol_10;
    equal_deltas->deltas[4]->movements = movement_left;

    // bits were different (0,1)
    equal_deltas->deltas[5] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[5]->state_name = program->state_count - 3;
    equal_deltas->deltas[5]->read_symbols = symbol_01;
    equal_deltas->deltas[5]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[5]->write_symbols = symbol_01;
    equal_deltas->deltas[5]->movements = movement_left;

    // all equal go back
    equal_deltas->deltas[6] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[6]->state_name = program->state_count - 2;
    equal_deltas->deltas[6]->read_symbols = symbol_0;
    equal_deltas->deltas[6]->subsequent_state = program->state_count - 2;
    equal_deltas->deltas[6]->write_symbols = symbol_0;
    equal_deltas->deltas[6]->movements = movement_left;

    // all equal go back
    equal_deltas->deltas[7] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[7]->state_name = program->state_count - 2;
    equal_deltas->deltas[7]->read_symbols = symbol_1;
    equal_deltas->deltas[7]->subsequent_state = program->state_count - 2;
    equal_deltas->deltas[7]->write_symbols = symbol_1;
    equal_deltas->deltas[7]->movements = movement_left;

    // bits different go back
    equal_deltas->deltas[8] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[8]->state_name = program->state_count - 1;
    equal_deltas->deltas[8]->read_symbols = symbol_0;
    equal_deltas->deltas[8]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[8]->write_symbols = symbol_0;
    equal_deltas->deltas[8]->movements = movement_left;

    // bits different go back
    equal_deltas->deltas[9] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[9]->state_name = program->state_count - 1;
    equal_deltas->deltas[9]->read_symbols = symbol_1;
    equal_deltas->deltas[9]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[9]->write_symbols = symbol_1;
    equal_deltas->deltas[9]->movements = movement_left;

    // bits different go back
    equal_deltas->deltas[10] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[10]->state_name = program->state_count - 1;
    equal_deltas->deltas[10]->read_symbols = symbol_01;
    equal_deltas->deltas[10]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[10]->write_symbols = symbol_01;
    equal_deltas->deltas[10]->movements = movement_left;

    // bits different go back
    equal_deltas->deltas[11] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[11]->state_name = program->state_count - 1;
    equal_deltas->deltas[11]->read_symbols = symbol_10;
    equal_deltas->deltas[11]->subsequent_state = program->state_count - 1;
    equal_deltas->deltas[11]->write_symbols = symbol_10;
    equal_deltas->deltas[11]->movements = movement_left;

    // all equal go back stop
    equal_deltas->deltas[12] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[12]->state_name = program->state_count - 2;
    equal_deltas->deltas[12]->read_symbols = symbol_blank;
    equal_deltas->deltas[12]->subsequent_state = subsequent_state_true;
    equal_deltas->deltas[12]->write_symbols = symbol_blank;
    equal_deltas->deltas[12]->movements = movement_stop;

    // bits different go back
    equal_deltas->deltas[13] = malloc(sizeof(struct delta *));
    equal_deltas->deltas[13]->state_name = program->state_count - 1;
    equal_deltas->deltas[13]->read_symbols = symbol_blank;
    equal_deltas->deltas[13]->subsequent_state = subsequent_state_false;
    equal_deltas->deltas[13]->write_symbols = symbol_blank;
    equal_deltas->deltas[13]->movements = movement_stop;

    return equal_deltas;
}


struct delta_group *bigger_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2) {

}

struct delta_group *bigger_equal_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2) {

}
