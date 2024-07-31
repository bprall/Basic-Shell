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
#include "../include/builtins/builtins.h"
#include "../include/utils.h"


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
    } else if (strcmp(command_line_words[0], "sort") == 0) {
        execute_sort_command(command_line_words, num_args);
    } else {
    execute_zip_commands(command_line_words, num_args);
    execute_forked_command(command_line_words, input_redirection, output_redirection, append_redirection, input_file, output_file);
    }
}   