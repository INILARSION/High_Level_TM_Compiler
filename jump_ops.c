#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jump_ops.h"
#include "helper.h"

struct delta_group *goto_operation(struct program *program, int start_state, int subsequent_state) {
    struct delta_group *goto_deltas = malloc(sizeof(struct delta_group));
    goto_deltas->delta_count = 1;
    goto_deltas->deltas = malloc(goto_deltas->delta_count * sizeof(struct delta *));
    goto_deltas->start_state = start_state;
    goto_deltas->subsequent_state = subsequent_state;

    char *movement_stop = malloc(program->head_count * sizeof(char ));
    int *symbol_blank = malloc(program->head_count * sizeof(int ));
    for (int i = 0; i < program->head_count; ++i) {
        movement_stop[i] = '-';
        symbol_blank[i] = 0;
    }

    goto_deltas->deltas[0] = malloc(sizeof(struct delta*));
    goto_deltas->deltas[0]->state_name = start_state;
    goto_deltas->deltas[0]->subsequent_state = subsequent_state;
    goto_deltas->deltas[0]->read_symbols = symbol_blank;
    goto_deltas->deltas[0]->write_symbols = symbol_blank;
    goto_deltas->deltas[0]->movements = movement_stop;

    return goto_deltas;
}

struct delta_group *if_operation(struct program *program, int start_state, char *compare_op, int band1, int band2, int jump_line_true, int jump_line_false) {

}