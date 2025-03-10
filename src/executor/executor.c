#include "../../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/executor/fork_exec.h"
#include "../../include/executor/pipe_exec.h"
#include "../../include/builtins/builtins.h"
#include "../../include/builtins/alias.h"
#include "../../include/io.h"
#include "../../include/utils.h"


int execute_command(char **command_line_words, size_t num_args) {
    int input_redirection = 0;
    int output_redirection = 0;
    int append_redirection = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    if (num_args < 1) {
        return 0;
    }

    handle_redirection(command_line_words, num_args, &input_redirection, &output_redirection, &append_redirection, &input_file, &output_file);

    if (strcmp(command_line_words[0], "history") == 0) {
        int num_to_show = (num_args > 1) ? atoi(command_line_words[1]) : 0;
        print_history(num_to_show);
        return 1;
    }

    const char *actual_command = get_command_for_alias(command_line_words[0]);
    if (actual_command != NULL) {
        char *new_command_line_words[MAX_COMMAND_LENGTH];
        size_t new_num_args = 0;

        char *command_copy = strdup(actual_command);
        char *token = strtok(command_copy, " ");
        while (token != NULL) {
            if (new_num_args < MAX_COMMAND_LENGTH - 1) {
                new_command_line_words[new_num_args++] = strdup(token);
            }
            token = strtok(NULL, " ");
        }
        free(command_copy);

        for (size_t i = 1; i < num_args; ++i) {
            if (new_num_args < MAX_COMMAND_LENGTH - 1) {
                new_command_line_words[new_num_args++] = strdup(command_line_words[i]);
            }
        }
        new_command_line_words[new_num_args] = NULL;

        num_args = new_num_args;
        command_line_words = new_command_line_words;
    }

    int result = execute_builtin_commands(command_line_words, num_args);
    if (result == -1) {
        result = execute_forked_command(command_line_words, input_redirection, output_redirection, append_redirection, input_file, output_file);
    }

    return result;
}

void execute_commands(char **command_line_words, size_t num_args) {
    size_t start_index = 0;
    int status = 1;
    int pipe_found = 0;

    for (size_t i = 0; i < num_args; i++) {
        if (strcmp(command_line_words[i], "|") == 0) {
            pipe_found = 1;
            break;
        }
    }

    if (pipe_found) {
        handle_pipes(command_line_words);
    } else {
        for (size_t i = 0; i < num_args; i++) {
            if (strcmp(command_line_words[i], "&&") == 0) {
                command_line_words[i] = NULL;
                size_t current_command_args = i - start_index;
                char **current_command = command_line_words + start_index;

                status = execute_command(current_command, current_command_args);
                if (status == 0) {
                    return;
                }

                start_index = i + 1;
            }
        }

        if (start_index < num_args) {
            char **last_command = command_line_words + start_index;
            size_t last_command_args = num_args - start_index;
            status = execute_command(last_command, last_command_args);
        }
    }
}