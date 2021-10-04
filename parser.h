#pragma once
#include "helper.h"

struct program *parse_program(char *program_file_path);

struct tapes *create_tapes(struct program *program);

void write_compiled_program(struct program *program, char *filename);

void write_tapes(struct program *program, struct tapes *tapes, char *filename);