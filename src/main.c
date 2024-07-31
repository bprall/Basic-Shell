#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include "../include/executor.h"
#include "../include/io.h"
#include "../include/utils.h"


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
            execute_command(command_line_words, num_args);
        }

        free_command(command_line_words);
    }

    free_history();

    return 0;
}