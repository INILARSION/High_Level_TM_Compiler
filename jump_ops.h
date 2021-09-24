#pragma once
#include "helper.h"

struct delta_group *goto_operation(struct program *program, int start_state, int subsequent_state);

struct delta_group *if_operation(struct program *program, int start_state, char *compare_op, int band1, int band2, int jump_line_true, int jump_line_false);
