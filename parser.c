#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "helper.h"

/*
 * Get the line count of a file.
 */
int get_file_line_count(FILE *file_ptr) {
    char *line = NULL;
    size_t buffer_size = 0;
    int line_count = 0;
    //count lines and reset to beginning of file
    while (getline(&line, &buffer_size, file_ptr) != -1) {
        ++line_count;
    }
    fseek(file_ptr, 0, 0);
    return line_count;
}

char *ltrim(char *string) {
    while(isspace(*string)) string++;
    return string;
}

char *rtrim(char *string)
{
    char* back = string + strlen(string);
    while(isspace(*--back));
    *(back+1) = '\0';
    return string;
}

char *trim(char *string) {
    return rtrim(ltrim(string));
}

/*
 * Check if line is emtpy/ only spaces or a comment
 */
int is_line_emtpy(char *line, int length) {
    // empty line
    if (length == 0)
        return 1;

    // comment or only spaces
    char *trimmed = strdup(line);
    trimmed = trim(trimmed);
    length = strlen(trimmed);
    int is_empty = 0;
    if (length == 0 || (length > 1 && trimmed[0] == '/' && trimmed[1] == '/')) {
        is_empty = 1;
    }
    free(trimmed);
    return is_empty;
}

/*
 * Get the number of initial variables and store it in init_vriables.
 * Return the line number where the initial variables end.
 */
int get_number_init_vars(FILE *file_ptr, struct init_variables *init_variables) {
    char *line = NULL;
    size_t buffer_size = 0;
    int init_vars = 0;
    char *temp_str;
    char *tmp;
    int line_length;
    int line_counter = 0;
    //count lines and reset to beginning of file
    while ((line_length = getline(&line, &buffer_size, file_ptr)) != -1) {
        ++line_counter;
        temp_str = strdup(line);
        if (is_line_emtpy(temp_str, line_length)) {
            free(temp_str);
            continue;
        }
        // check if + / incr / decr / goto or if statement
        if (strstr(temp_str, "+") != NULL || strstr(temp_str, "incr") != NULL || strstr(temp_str, "decr") != NULL || strstr(temp_str, "goto") != NULL || strstr(temp_str, "if") != NULL) {
            free(temp_str);
            break;
        }
        // check if - operator
        if ((tmp = strstr(temp_str, "-")) != NULL) {
            if (tmp[-1] == ' ' && tmp[1] == ' ') {
                free(temp_str);
                break;
            }
        }
        free(temp_str);
        ++init_vars;
    }
    fseek(file_ptr, 0, 0);
    init_variables->variable_count = init_vars;
    return line_counter - 1;
}

/*
 * Get the index of a variable in the init_variables list
 */
int get_var_index(struct init_variables *init_variables, char *var_name) {
    for (int i = 0; i < init_variables->variable_count; ++i) {
        if (strcmp(init_variables->variable_names[i], var_name) == 0)
            return i;
    }
    return -1;
}

void parse_comparison(struct init_variables *init_variables, struct conditional_jump *conditional_jump, char *line) {
    line = trim(line);
    int line_length = strlen(line);
    int end = 0;
    for (int i = 0; i < line_length; ++i) {
        if (!isalpha(line[i])) {
            end = i;
            break;
        }
    }
    char *band_1 = strndup(line, end);
    conditional_jump->band_1 = get_var_index(init_variables, band_1);
    line += end;
    line_length -= end;
    for (int i = 0; i < line_length; ++i) {
        if (isalpha(line[i])) {
            end = i;
            break;
        }
    }
    conditional_jump->compare_op = trim(strndup(line, end));
    line += end;
    conditional_jump->band_2 = get_var_index(init_variables, trim(line));
}

/*
 * Parse the file containing the Program and produce a struct containing its information.
 */
struct program *parse_program(char *program_file_path) {
    FILE *file_ptr = fopen(program_file_path, "r");
    size_t line_length;
    char *line = NULL;
    size_t buffer_size = 0;
    struct program *program = malloc(sizeof(struct program));
    struct init_variables *init_variables = malloc(sizeof(struct init_variables));

    // check that file is found
    if(file_ptr == NULL) {
        printf("File %s not found!", program_file_path);
        exit(1);
    }

    //count lines and reset to beginning of file
    int line_count = get_file_line_count(file_ptr);

    int init_end_line = get_number_init_vars(file_ptr, init_variables);
    init_variables->variable_names = malloc(init_variables->variable_count * sizeof(char *));
    init_variables->variable_init_values = malloc(init_variables->variable_count * sizeof(int));


    char *temp_str;
    char *temp_str_ptr;
    char *var;
    int var_counter = 0;
    // parse the init variables
    for (int i = 0; i < init_end_line; i++) {
        line_length = getline(&line, &buffer_size, file_ptr);
       temp_str_ptr =  temp_str = strdup(line);

        if (is_line_emtpy(temp_str, line_length)) {
            free(temp_str_ptr);
            continue;
        }

        if (strstr(temp_str, "=") == NULL) {
            printf("Line %d has a wrong format!", i);
            exit(-1);
        }
        var = strsep(&temp_str, "=");
        init_variables->variable_names[var_counter] = strdup(trim(var));
        init_variables->variable_init_values[var_counter] = atoi(trim(temp_str));
        ++var_counter;
        free(temp_str_ptr);
    }

    program->line_count = line_count - init_end_line;
    struct line **lines = malloc(program->line_count * sizeof(struct line *));

    // parse the rest
    for (int i = init_end_line; i < line_count; i++) {
        line_length = getline(&line, &buffer_size, file_ptr);
        temp_str_ptr = temp_str = strdup(line);
        struct line *line_struct = malloc(sizeof(struct line));
        line_struct->line_number = i + 1;

        if (is_line_emtpy(temp_str, line_length)) {
            line_struct->type = 'e';
            line_struct->operation_struct = NULL;
        } else if (strstr(temp_str, "+") != NULL) {
            line_struct->type = 'a';
            struct add *add = malloc(sizeof(struct add));
            var = strsep(&temp_str, "=");
            add->sum_band = get_var_index(init_variables, trim(var));
            temp_str = trim(temp_str);
            char *augend = strdup(strsep(&temp_str, "+"));
            add->augend_band = get_var_index(init_variables, trim(augend));
            add->addend_band = get_var_index(init_variables, trim(temp_str));
            line_struct->operation_struct = add;
        } else if (strstr(temp_str, "incr") != NULL) {
            line_struct->type = 'i';
            struct incr *incr = malloc(sizeof(struct incr));
            strsep(&temp_str, " ");
            incr->incr_band = get_var_index(init_variables, trim(temp_str));
            line_struct->operation_struct = incr;
        } else if (strstr(temp_str, "decr") != NULL) {
            line_struct->type = 'd';
            struct decr *decr = malloc(sizeof(struct decr));
            strsep(&temp_str, " ");
            decr->decr_band = get_var_index(init_variables, trim(temp_str));
            line_struct->operation_struct = decr;
        } else if (strstr(temp_str, "goto") != NULL) {
            line_struct->type = 'g';
            struct jump *jump = malloc(sizeof(struct jump));
            strsep(&temp_str, " ");
            jump->jump_line_number = atoi(trim(temp_str));
            line_struct->operation_struct = jump;
        } else if (strstr(temp_str, "if") != NULL) {
            line_struct->type = 'c';
            struct conditional_jump *conditional_jump = malloc(sizeof(struct conditional_jump));
            strsep(&temp_str, " ");
            char *operation = strdup(strsep(&temp_str, "then"));
            temp_str += 3;
            parse_comparison(init_variables, conditional_jump, operation);
            conditional_jump->jump_line_true = atoi(trim(strdup(strsep(&temp_str, "else"))));
            temp_str += 3;
            conditional_jump->jump_line_false = atoi(trim(temp_str));

            line_struct->operation_struct = conditional_jump;
        } else if (strstr(temp_str, "-") != NULL) {
            line_struct->type = 's';
            struct sub *sub = malloc(sizeof(struct sub));
            var = strsep(&temp_str, "=");
            sub->difference_band = get_var_index(init_variables, trim(var));
            temp_str = trim(temp_str);
            char *minuend = strdup(strsep(&temp_str, "-"));
            sub->minuend_band = get_var_index(init_variables, trim(minuend));
            sub->subtrahend_band = get_var_index(init_variables, trim(temp_str));
            line_struct->operation_struct = sub;
        } else {
            printf("Unknown operation in line %d!\n", i);
        }
        lines[i - init_end_line] = line_struct;
        free(temp_str_ptr);
    }


    if (line)
        free(line);
    fclose(file_ptr);

    program->state_count = 3;
    program->state_names = malloc(program->state_count * sizeof(char *));
    program->state_names[0] = calloc(int_length(lines[0]->line_number) + 1, sizeof(char ));
    sprintf(program->state_names[0], "L%d", lines[0]->line_number);
    program->state_names[1] = strdup("accept");
    program->state_names[2] = strdup("reject");
    program->head_count = init_variables->variable_count + 1;
    program->alphabet = strdup(" 01");
    program->alphabet_size = 3;
    program->lines = lines;
    program->init_variables = init_variables;

    return program;
}


struct tapes *create_tapes(struct program *program) {
    struct tapes *tapes = malloc(sizeof(struct tapes));
    tapes->tape_count = program->head_count;
    tapes->tapes = malloc(program->head_count * sizeof(struct tape*));
    int int_size = (sizeof(int) * 8);
    char *buffer;

    struct tape *current_tape;
    for (int i = 0; i < program->init_variables->variable_count; ++i) {
        current_tape = malloc(sizeof(struct tape));
        buffer = calloc(int_size + 1, sizeof(char ));
        int_to_bin(program->init_variables->variable_init_values[i], buffer, int_size);
        current_tape->length = int_size;
        current_tape->tape_arr = calloc(int_size + 2, sizeof(int ));
        current_tape->tape_arr[0] = 0;
        for (int j = 0; j < int_size; ++j) {
            // Buffer is a string of 0 and 1
            // '0' should be int value 1 and '1' int value 2
            current_tape->tape_arr[j + 1] = ((int)buffer[j]) - 47;
        }
        tapes->tapes[i] = current_tape;
    }
    // create empty tape, for temporary values for operations
    current_tape = malloc(sizeof(struct tape));
    current_tape->length = 1;
    current_tape->tape_arr = calloc(int_size + 1, sizeof(int ));
    current_tape->tape_arr[0] = 0;
    tapes->tapes[program->head_count - 1] = current_tape;
    return tapes;
}

/*
 * write one delta to file
 */
void print_delta(struct program *program, struct delta *delta, FILE *file_ptr) {
    fprintf(file_ptr, "D: %s,", program->state_names[delta->state_name]);

    for (int i = 0; i < program->head_count; ++i) {
        fprintf(file_ptr, "%c", program->alphabet[delta->read_symbols[i]]);
        if (i < program->head_count - 1)
            fprintf(file_ptr, "|");
    }

    fprintf(file_ptr, ",%s,", program->state_names[delta->subsequent_state]);

    for (int i = 0; i < program->head_count; ++i) {
        fprintf(file_ptr, "%c", program->alphabet[delta->write_symbols[i]]);
        if (i < program->head_count - 1)
            fprintf(file_ptr, "|");
    }

    fprintf(file_ptr, ",");

    for (int i = 0; i < program->head_count; ++i) {
        fprintf(file_ptr, "%c", delta->movements[i]);
        if (i < program->head_count - 1)
            fprintf(file_ptr, "|");
    }

    fprintf(file_ptr, "\n");
}

/*
 * Write the compiled program to a file.
 */
void write_compiled_program(struct program *program, char *filename) {
    FILE *file_ptr = fopen(filename, "w");

    // write states
    fprintf(file_ptr, "S: ");
    for (int i = 0; i < program->state_count - 1; ++i) {
        fprintf(file_ptr, "%s,", program->state_names[i]);
    }
    fprintf(file_ptr, "%s\n", program->state_names[program->state_count - 1]);

    //write alphabet
    fprintf(file_ptr, "G: ");
    for (int i = 0; i < program->alphabet_size - 1; ++i) {
        fprintf(file_ptr, "%c,", program->alphabet[i]);
    }
    fprintf(file_ptr, "%c\n", program->alphabet[program->alphabet_size - 1]);

    struct line **lines = program->lines;
    struct delta_group *deltas;
    // write deltas
    for (int i = 0; i < program->line_count; ++i) {
        if(lines[i]->type == 'e')
            continue;
        deltas = lines[i]->deltas;
        for (int j = 0; j < deltas->delta_count; ++j) {
            print_delta(program, deltas->deltas[j], file_ptr);
        }
    }

    fclose(file_ptr);
}

/*
 * Write the tapes to a file
 */
void write_tapes(struct program *program, struct tapes *tapes, char *filename) {
    FILE *file_ptr = fopen(filename, "w");
    for (int i = 0; i < tapes->tape_count; ++i) {
        for (int j = 0; j < tapes->tapes[i]->length; ++j) {
            fprintf(file_ptr, "%c", program->alphabet[tapes->tapes[i]->tape_arr[j]]);
            if (j < tapes->tapes[i]->length - 1)
                fprintf(file_ptr, "|");
        }
        if (i < tapes->tape_count - 1)
            fprintf(file_ptr, "\n");
    }
    fclose(file_ptr);
}