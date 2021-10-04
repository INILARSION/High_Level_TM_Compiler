#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jump_ops.h"
#include "helper.h"
#include "cmp_ops.h"

struct delta_group *goto_operation(struct program *program, int start_state, int subsequent_state) {
    struct delta_group *goto_deltas = malloc(sizeof(struct delta_group));
    goto_deltas->delta_count = 1;
    goto_deltas->deltas = malloc(goto_deltas->delta_count * sizeof(struct delta *));

    char *movement_stop = malloc(program->head_count * sizeof(char ));
    int *symbol_blank = malloc(program->head_count * sizeof(int ));
    for (int i = 0; i < program->head_count; ++i) {
        movement_stop[i] = '-';
        symbol_blank[i] = 0;
    }

    goto_deltas->deltas[0] = malloc(sizeof(struct delta));
    goto_deltas->deltas[0]->state_name = start_state;
    goto_deltas->deltas[0]->subsequent_state = subsequent_state;
    goto_deltas->deltas[0]->read_symbols = symbol_blank;
    goto_deltas->deltas[0]->write_symbols = symbol_blank;
    goto_deltas->deltas[0]->movements = movement_stop;

    return goto_deltas;
}

struct delta_group *if_operation(struct program *program, int start_state, char *compare_op, int jump_line_true, int jump_line_false, int band1, int band2) {
    struct delta_group *if_deltas;
    if (strcmp(compare_op, "==") == 0) {
        if_deltas = equal_operation(program, start_state, jump_line_true, jump_line_false, band1, band2);
    } else if (strcmp(compare_op, "!=") == 0) {
        if_deltas = equal_operation(program, start_state, jump_line_false, jump_line_true, band1, band2);
    } else if (strcmp(compare_op, "<") == 0) {
        if_deltas = bigger_operation(program, start_state, jump_line_false, jump_line_true, band1, band2);
    } else if (strcmp(compare_op, "<=") == 0) {
        if_deltas = bigger_equal_operation(program, start_state, jump_line_false, jump_line_true, band1, band2);
    } else if (strcmp(compare_op, ">") == 0) {
        if_deltas = bigger_operation(program, start_state, jump_line_true, jump_line_false, band1, band2);
    }else if (strcmp(compare_op, ">=") == 0) {
        if_deltas = bigger_equal_operation(program, start_state, jump_line_true, jump_line_false, band1, band2);
    } else {
        printf("Wrong comparisson operator!\n");
        exit(-1);
    }
    return if_deltas;
}