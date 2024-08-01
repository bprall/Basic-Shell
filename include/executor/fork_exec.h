#ifndef FORK_EXEC_H
#define FORK_EXEC_H

int execute_forked_command(char **command_line_words, int input_redirection, int output_redirection, int append_redirection, char *input_file, char *output_file);

#endif // FORK_EXEC_H