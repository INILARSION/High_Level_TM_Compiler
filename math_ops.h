#pragma once
#include "helper.h"

struct delta_group *add_operation(struct program *program, int start_state, int subsequent_state, int augend_band, int addend_band, int sum_band);

struct delta_group *sub_operation(struct program *program, int start_state, nt subsequent_state, int minuend_band, int subtrahend_band, int difference_band);

struct delta_group * write_number_to_tape(struct program *program, int start_state, int subsequent_state, int band, int number);

struct delta_group *incr_operation(struct program *program, int start_state, int subsequent_state, int incr_band);

struct delta_group *decr_operation(struct program *program, int start_state, int subsequent_state, int incr_band);
