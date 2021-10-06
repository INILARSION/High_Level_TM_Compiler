#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler_core.h"
#include "helper.h"
#include "math_ops.h"
#include "jump_ops.h"

int create_line_state(struct program *program, int line_number) {
    char *line_str = calloc(int_length(line_number) + 2, sizeof(char ));
    sprintf(line_str, "L%d", line_number);
    program->state_count++;
    program->state_names = realloc(program->state_names, program->state_count * sizeof(char *));
    program->state_names[program->state_count - 1] = line_str;
    return program->state_count - 1;
}

int find_line_state(struct program *program, int line_number) {
    char *line_str = calloc(int_length(line_number) + 2, sizeof(char ));
    sprintf(line_str, "L%d", line_number);
    for (int i = 0; i < program->state_count; ++i) {
        if (strcmp(program->state_names[i], line_str) == 0) {
            free(line_str);
            return i;
        }
    }
    free(line_str);
    return create_line_state(program, line_number);
}

int find_line_subsequent_state(struct program *program, int line_number) {
    // return accept state if last line
    if (line_number - program->init_variables->variable_count > program->line_count)
        return 1;
    else
        return find_line_state(program, line_number);
}

void compile_program(struct program *program) {
    struct line *line;
    int state_name;
    int subsequent_state;
    int subsequent_state_2;
    for (int i = 0; i < program->line_count; ++i) {
        line = program->lines[i];
        switch (line->type) {
            case 'e':
                state_name = find_line_state(program, line->line_number);
                subsequent_state = find_line_subsequent_state(program, line->line_number + 1);
                line->deltas = goto_operation(program, state_name, subsequent_state);
                break;
            case 'a':
                state_name = find_line_state(program, line->line_number);
                subsequent_state = find_line_subsequent_state(program, line->line_number + 1);
                struct add *add = line->operation_struct;
                line->deltas = add_operation(program, state_name, subsequent_state, add->augend_band, add->addend_band, add->sum_band);
                break;
            case 's':
                state_name = find_line_state(program, line->line_number);
                subsequent_state = find_line_subsequent_state(program, line->line_number + 1);
                struct sub *sub = line->operation_struct;
                line->deltas = sub_operation(program, state_name, subsequent_state, sub->minuend_band, sub->subtrahend_band, sub->difference_band);
                break;
            case 'i':
                state_name = find_line_state(program, line->line_number);
                subsequent_state = find_line_subsequent_state(program, line->line_number + 1);
                struct incr *incr = line->operation_struct;
                line->deltas = incr_operation(program, state_name, subsequent_state, incr->incr_band);
                break;
            case 'd':
                state_name = find_line_state(program, line->line_number);
                subsequent_state = find_line_subsequent_state(program, line->line_number + 1);
                struct decr *decr = line->operation_struct;
                line->deltas = decr_operation(program, state_name, subsequent_state, decr->decr_band);
                break;
            case 'c':
                state_name = find_line_state(program, line->line_number);
                struct conditional_jump *conditional_jump = line->operation_struct;
                subsequent_state = find_line_subsequent_state(program, conditional_jump->jump_line_true);
                subsequent_state_2 = find_line_subsequent_state(program, conditional_jump->jump_line_false);
                // comparing a var to itself is always equal
                if (conditional_jump->band_1 == conditional_jump->band_2 && strlen(conditional_jump->compare_op) == 2 && conditional_jump->compare_op[1] == '=') {
                    line->deltas = goto_operation(program, state_name, subsequent_state);
                } else {
                    line->deltas = if_operation(program, state_name, conditional_jump->compare_op, subsequent_state, subsequent_state_2, conditional_jump->band_1, conditional_jump->band_2);
                }
                break;
            case 'g':
                state_name = find_line_state(program, line->line_number);
                struct jump *jump = line->operation_struct;
                subsequent_state = find_line_subsequent_state(program, jump->jump_line_number);
                line->deltas = goto_operation(program, state_name, subsequent_state);
                break;
        }
    }
}