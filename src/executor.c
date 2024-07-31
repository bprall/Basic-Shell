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
#include "../include/io.h"
#include "../include/builtins.h"

#define MAX_ARGS 32


void handle_pipes(char **command_line_words, int pipe_fd[]) {
    char **left_cmd = command_line_words;
    char **right_cmd = NULL;
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

        if (fork() == 0) {
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execvp(left_cmd[0], left_cmd);
            perror("execvp");
            exit(1);
        }

        if (fork() == 0) {
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            execvp(right_cmd[0], right_cmd);
            perror("execvp");
            exit(1);
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);
        wait(NULL);
        wait(NULL);
    }
}

void execute_forked_command(char **command_line_words, int input_redirection, int output_redirection, int append_redirection,
                            char *input_file, char *output_file) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
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
            exit(1);
        }
    }
}

void execute_zip_commands(char **command_line_words, size_t num_args) {
    if (strcmp(command_line_words[0], "zip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            compress(input_file, output_file);
        } else {
            printf("Usage: zip <input_file> <output_file>\n");
        }
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            uncompress(input_file, output_file);
        } else {
            printf("Usage: unzip <input_file> <output_file>\n");
        }
    }
}

void execute_help_command(char **command_line_words, size_t num_args) {
    if (num_args > 1) {
        cmd_help(command_line_words[1]);
    } else {
        cmd_help(NULL);
    }
}

void execute_wc_command(char **command_line_words, size_t num_args) {
    int show[3] = {1, 1, 1};
    int total_counts[3] = {0};
    int file_count = 0;

    for (size_t i = 1; i < num_args; i++) {
        if (command_line_words[i][0] == '-') {
            if (strcmp(command_line_words[i], "-l") == 0) {
                show[1] = show[2] = 0;
            } else if (strcmp(command_line_words[i], "-w") == 0) {
                show[0] = show[2] = 0;
            } else if (strcmp(command_line_words[i], "-c") == 0) {
                show[0] = show[1] = 0;
            } else {
                printf("Invalid argument\n");
            }
        } else {
            int *counts = get_counts(command_line_words[i]);
            if (counts != NULL) {
                print_counts(show, counts, command_line_words[i]);
                for (int j = 0; j < 3; j++) {
                    total_counts[j] += counts[j];
                }
                free(counts);
                file_count++;
            }
        }
    }

    if (file_count == 0) {
        int *counts = get_counts("");
        if (counts != NULL) {
            print_counts(show, counts, "");
            free(counts);
        }
    } else if (file_count > 1) {
        print_counts(show, total_counts, "total");
    }
}

void execute_command(char **command_line_words, size_t num_args) {
    int input_redirection = 0;
    int output_redirection = 0;
    int append_redirection = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    int pipe_fd[2] = {-1, -1};

    if (num_args > 0 && strcmp(command_line_words[0], "help") == 0) {
        execute_help_command(command_line_words, num_args);
        return; 
    }
    handle_pipes(command_line_words, pipe_fd);
    handle_redirection(command_line_words, num_args, &input_redirection, &output_redirection, &append_redirection, &input_file, &output_file);
    if (strcmp(command_line_words[0], "wc") == 0) {
        execute_wc_command(command_line_words, num_args);
    } else {
    execute_zip_commands(command_line_words, num_args);
    execute_forked_command(command_line_words, input_redirection, output_redirection, append_redirection, input_file, output_file);
    }
}   