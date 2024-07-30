#ifndef IO_H
#define IO_H

#include <stddef.h>

void handle_redirection(char **command_line_words, size_t num_args, int *input_redirection, int *output_redirection, int *append_redirection, char **input_file, char **output_file);
char **get_next_command(size_t *num_args);

#endif // IO_H