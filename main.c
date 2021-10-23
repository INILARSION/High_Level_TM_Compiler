#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "parser.h"
#include "compiler_core.h"

int main(int argc, char** argv) {
    char *program_file_path;
    char *tm_file_path;
    char *tape_file_path;

    if (argc == 3) {
        program_file_path = argv[1];
        tm_file_path = calloc(strlen(argv[2]) + 7, sizeof(char));
        sprintf(tm_file_path, "%s.delta", argv[2]);
        tape_file_path = calloc(strlen(argv[2]) + 6, sizeof(char));
        sprintf(tape_file_path, "%s.tape", argv[2]);
    } else {
        printf("Usage: %s [Program file path] [Output file path]", argv[0]);
        return -1;
    }

    struct program *program = parse_program(program_file_path);

    struct tapes *tapes = create_tapes(program);

    compile_program(program);

    write_compiled_program(program, tm_file_path);

    write_tapes(program, tapes, tape_file_path);

    return 0;
}

