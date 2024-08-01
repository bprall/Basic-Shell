#ifndef EXECUTOR_H
#define EXECUTOR_H

int execute_command(char **command_line_words, size_t num_args);
void execute_commands(char **command_line_words, size_t num_args);

#endif // EXECUTOR_H