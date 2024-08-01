#include "../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stddef.h>

#include "../include/builtins/builtins.h"
#include "../include/builtins/alias.h"
#include "../include/io.h"
#include "../include/utils.h"


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
        result = 1;
    } else if (strcmp(command_line_words[0], "grep") == 0) {
        result = execute_grep_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "sort") == 0) {
        result = execute_sort_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "zip") == 0) {
        result = execute_zip_commands(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        result = execute_zip_commands(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "sortwords") == 0) {
        result = execute_sortwords_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "alias") == 0) {
        result = execute_alias_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "unalias") == 0) {
        result = remove_alias(command_line_words[1]);
    } else {
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