#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stddef.h>

int execute_forked_command(char **command_line_words, int input_redirection, int output_redirection, int append_redirection, char *input_file, char *output_file);
void handle_pipes(char **command_line_words);
int execute_command(char **command_line_words, size_t num_args);
void execute_commands(char **command_line_words, size_t num_args);

#endif // EXECUTOR_H