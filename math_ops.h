#pragma once
#include "helper.h"

struct delta_group *add_operation(struct program *program, int start_state, int augend_band, int addend_band, int sum_band);

struct delta_group *mult_operation(struct program *program, int start_state, int multiplier_band, int multiplicand_band, int product_band);

struct delta_group *sub_operation(struct program *program, int start_state, int minuend_band, int subtrahend_band, int difference_band);

struct delta_group *incr_operation(struct program *program, int start_state, int incr_band);
