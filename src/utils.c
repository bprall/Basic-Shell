#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 32

void free_command(char **words) {
    for (int i = 0; i < MAX_ARGS; ++i) {
        if (words[i] == NULL) {
            break;
        }
        free(words[i]);
    }
    free(words);
}

void sigchld_handler(int signo) {
    (void) signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}