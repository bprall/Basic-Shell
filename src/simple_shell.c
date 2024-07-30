#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include "../include/zip.h"
#include "../include/unzip.h"

#define MAX_ARGS 32
#define HISTORY_SIZE 100

char *history[HISTORY_SIZE];
int history_count = 0;

void sigchld_handler(int signo) {
    (void) signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

void add_to_history(const char *command) {
    if (history_count < HISTORY_SIZE) {
        history[history_count] = strdup(command);
        history_count++;
    } else {
        free(history[0]);
        memmove(history, history + 1, (HISTORY_SIZE - 1) * sizeof(char *));
        history[HISTORY_SIZE - 1] = strdup(command);
    }
}

void print_history() {
    for (int i = 0; i < history_count; ++i) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

char **get_next_command(size_t *num_args) {
    printf("cssh$ ");

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    if (ferror(stdin)) {
        perror("getline");
        exit(1);
    }
    if (feof(stdin)) {
        free(line);
        return NULL;
    }

    add_to_history(line);

    char **words = (char **)malloc(MAX_ARGS * sizeof(char *));
    int i = 0;

    char *parse = line;
    while (parse != NULL) {
        char *word = strsep(&parse, " \t\r\f\n");
        if (strlen(word) != 0) {
            words[i++] = strdup(word);
        }
    }
    *num_args = i;
    for (; i < MAX_ARGS; ++i) {
        words[i] = NULL;
    }

    free(line);

    return words;
}

void free_command(char **words) {
    for (int i = 0; i < MAX_ARGS; ++i) {
        if (words[i] == NULL) {
            break;
        }
        free(words[i]);
    }
    free(words);
}

void execute_command(char **command_line_words, size_t num_args) {
    int input_redirection = 0;
    int output_redirection = 0;
    int append_redirection = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    int pipe_fd[2] = {-1, -1};

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
        return;
    }

    // Handle redirection
    for (int i = 0; command_line_words[i] != NULL; i++) {
        if (strcmp(command_line_words[i], "<") == 0) {
            if (input_redirection) {
                printf("Error! Can't have two <'s!\n");
                return;
            }
            input_redirection = 1;
            input_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        } else if (strcmp(command_line_words[i], ">") == 0) {
            if (output_redirection || append_redirection) {
                printf("Error! Can't have two >'s or >>'s!\n");
                return;
            }
            output_redirection = 1;
            output_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        } else if (strcmp(command_line_words[i], ">>") == 0) {
            if (output_redirection || append_redirection) {
                printf("Error! Can't have two >'s or >>'s!\n");
                return;
            }
            append_redirection = 1;
            output_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        }
    }

    if (strcmp(command_line_words[0], "zip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            compress(input_file, output_file); // call the compress function
        } else {
            printf("Usage: zip <input_file> <output_file>\n");
        }
        return;
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            uncompress(input_file, output_file); // call the uncompress function
        } else {
            printf("Usage: unzip <input_file> <output_file>\n");
        }
        return;
    }

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

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    size_t num_args;
    char **command_line_words;

    while ((command_line_words = get_next_command(&num_args)) != NULL) {
        if (command_line_words[0] == NULL) {
            free_command(command_line_words);
            continue;
        }

        if (strcmp(command_line_words[0], "exit") == 0) {
            free_command(command_line_words);
            break;
        } else if (strcmp(command_line_words[0], "cd") == 0) {
            if (num_args > 1) {
                if (chdir(command_line_words[1]) != 0) {
                    perror("chdir");
                }
            } else {
                printf("cd: missing argument\n");
            }
        } else if (strcmp(command_line_words[0], "pwd") == 0) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd");
            }
        } else if (strcmp(command_line_words[0], "history") == 0) {
            print_history();
        } else {
            execute_command(command_line_words, num_args); // Pass num_args to execute_command
        }

        free_command(command_line_words);
    }

    for (int i = 0; i < history_count; ++i) {
        free(history[i]);
    }

    return 0;
}
