#ifndef BUILTINS_H
#define BUILTINS_H

int execute_zip_commands(char **command_line_words, size_t num_args);
int execute_help_command(char **command_line_words, size_t num_args);
int execute_sort_command(char **command_line_words, size_t num_args);
int execute_wc_command(char **command_line_words, size_t num_args);
int execute_sortwords_command(char **command_line_words, size_t num_args);
int execute_alias_command(char **command_line_words, size_t num_args);
int remove_alias(const char *alias);
int execute_grep_command(char **command_line_words, size_t num_args);

#endif // BUILTINS_H