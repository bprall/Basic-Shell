#include "../../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


void handle_pipes(char **command_line_words) {
    int pipe_count = 0;
    int i = 0;
    
    while (command_line_words[i] != NULL) {
        if (strcmp(command_line_words[i], "|") == 0) {
            pipe_count++;
        }
        i++;
    }
    
    int pipe_fd[pipe_count * 2];
    for (i = 0; i < pipe_count; i++) {
        if (pipe(pipe_fd + i * 2) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    char **cmd_start = command_line_words;
    char **cmd_end = command_line_words;
    
    for (i = 0; i <= pipe_count; i++) {
        if (i < pipe_count) {
            cmd_end = command_line_words;
            while (*cmd_end != NULL && strcmp(*cmd_end, "|") != 0) {
                cmd_end++;
            }
            *cmd_end = NULL;
        } else {
            cmd_end = command_line_words + i * 2;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            if (i > 0) {
                dup2(pipe_fd[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < pipe_count) {
                dup2(pipe_fd[i * 2 + 1], STDOUT_FILENO);
            }

            for (int j = 0; j < pipe_count * 2; j++) {
                close(pipe_fd[j]);
            }

            execvp(cmd_start[0], cmd_start);
            perror("execvp");
            exit(1);
        }

        cmd_start = cmd_end + 1;

        if (i > 0) {
            close(pipe_fd[(i - 1) * 2]);
        }
        if (i < pipe_count) {
            close(pipe_fd[i * 2 + 1]);
        }
    }

    for (i = 0; i < pipe_count * 2; i++) {
        close(pipe_fd[i]);
    }

    int status;
    for (i = 0; i <= pipe_count; i++) {
        wait(&status);
    }
}