#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "../include/io.h"
#include "../include/builtins/builtins.h"
#include "../include/utils.h"


void handle_pipes(char **command_line_words) {
    char **left_cmd = command_line_words;
    char **right_cmd = NULL;
    int pipe_fd[2];
    int pipe_index = -1;

    for (int i = 0; command_line_words[i] != NULL; i++) {
        if (strcmp(command_line_words[i], "|") == 0) {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index != -1) {
        command_line_words[pipe_index] = NULL;
        right_cmd = &command_line_words[pipe_index + 1];
        pipe(pipe_fd);

        pid_t pid1 = fork();
        if (pid1 == 0) {
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execvp(left_cmd[0], left_cmd);
            perror("execvp");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execvp(right_cmd[0], right_cmd);
            perror("execvp");
            exit(1);
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
}

int execute_forked_command(char **command_line_words, int input_redirection, int output_redirection, int append_redirection,
                            char *input_file, char *output_file) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 0; 
    } else if (pid == 0) {
        if (input_redirection) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            if (dup2(fd, STDIN_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(fd);
        }
        if (output_redirection) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(fd);
        }
        if (append_redirection) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(fd);
        }

        execvp(command_line_words[0], command_line_words);
        perror("execvp");
        exit(1);
    } else {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            return 0;
        }
    }
    return 1; 
}

int execute_command(char **command_line_words, size_t num_args) {
    int input_redirection = 0;
    int output_redirection = 0;
    int append_redirection = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    if (num_args > 0 && strcmp(command_line_words[0], "help") == 0) {
        return execute_help_command(command_line_words, num_args);
    }

    handle_redirection(command_line_words, num_args, &input_redirection, &output_redirection, &append_redirection, &input_file, &output_file);

    char **cmd_with_pipes = malloc((num_args + 1) * sizeof(char *));
    if (cmd_with_pipes == NULL) {
        perror("malloc");
        return 0; 
    }
    memcpy(cmd_with_pipes, command_line_words, num_args * sizeof(char *));
    cmd_with_pipes[num_args] = NULL;

    if (strchr((char *)command_line_words, '|')) {
        handle_pipes(cmd_with_pipes);
        free(cmd_with_pipes);
        return 1; 
    }

    int result;
    if (strcmp(command_line_words[0], "wc") == 0) {
        result = execute_wc_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "cd") == 0) {
        if (num_args > 1) {
            if (chdir(command_line_words[1]) != 0) {
                perror("chdir");
                result = 0;
            } else {
                result = 1;
            }
        } else {
            printf("cd: missing argument\n");
            result = 0;
        }
    } else if (strcmp(command_line_words[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            result = 1;
        } else {
            perror("getcwd");
            result = 0;
        }
    } else if (strcmp(command_line_words[0], "history") == 0) {
        int num_to_show = (num_args > 1) ? atoi(command_line_words[1]) : 0;
        print_history(num_to_show);
        result = 1; // Indicate success
    } else if (strcmp(command_line_words[0], "sort") == 0) {
        result = execute_sort_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "zip") == 0) {
        result = execute_zip_commands(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        result = execute_zip_commands(command_line_words, num_args);
    } else {
        result = execute_forked_command(command_line_words, input_redirection, output_redirection, append_redirection, input_file, output_file);
    }

    free(cmd_with_pipes);
    return result;
}

void execute_commands(char **command_line_words, size_t num_args) {
    size_t start_index = 0;
    int status = 1;

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