#include "../include/config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

#include "../include/executor/executor.h"
#include "../include/io.h"
#include "../include/utils.h"


void sigchld_handler(int signo) {
    (void) signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

void free_command(char **words, size_t num_args) {
    for (size_t i = 0; i < num_args; ++i) {
        if (words[i] != NULL) {
            free(words[i]);
        }
    }
    free(words);
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
            free_command(command_line_words, num_args);
            continue;
        }

        if (strcmp(command_line_words[0], "exit") == 0) {
            free_command(command_line_words, num_args);
            break;
        } else {
            execute_commands(command_line_words, num_args);
        }

        free_command(command_line_words, num_args);
    }

    free_history();

    return 0;
}