#ifndef BUILTINS_H
#define BUILTINS_H

void execute_zip_commands(char **command_line_words, size_t num_args);
void execute_help_command(char **command_line_words, size_t num_args);
void execute_sort_command(char **command_line_words, size_t num_args);
void execute_wc_command(char **command_line_words, size_t num_args);

#endif // BUILTINS_H