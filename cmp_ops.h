#pragma once
#include "helper.h"

struct delta_group *equal_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2);

struct delta_group *bigger_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2);

struct delta_group *bigger_equal_operation(struct program *program, int start_state, int subsequent_state_true, int subsequent_state_false, int band_1, int band_2);