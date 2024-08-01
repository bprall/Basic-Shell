#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/config.h"

#define HISTORY_SIZE 100


char *history[HISTORY_SIZE];
int history_count = 0;


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

void print_history(int count) {
    if (count <= 0 || count > history_count) {
        count = history_count; 
    }

    for (int i = history_count - count; i < history_count; ++i) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void free_history() {
    for (int i = 0; i < history_count; ++i) {
        free(history[i]);
    }
}

void handle_redirection(char **command_line_words, size_t num_args,
                        int *input_redirection, int *output_redirection, int *append_redirection,
                        char **input_file, char **output_file) {
    for (int i = 0; command_line_words[i] != NULL; i++) {
        if (strcmp(command_line_words[i], "<") == 0) {
            if (*input_redirection) {
                printf("Error! Can't have two <'s!\n");
                return;
            }
            *input_redirection = 1;
            *input_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        } else if (strcmp(command_line_words[i], ">") == 0) {
            if (*output_redirection || *append_redirection) {
                printf("Error! Can't have two >'s or >>'s!\n");
                return;
            }
            *output_redirection = 1;
            *output_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        } else if (strcmp(command_line_words[i], ">>") == 0) {
            if (*output_redirection || *append_redirection) {
                printf("Error! Can't have two >'s or >>'s!\n");
                return;
            }
            *append_redirection = 1;
            *output_file = command_line_words[i + 1];
            command_line_words[i] = NULL;
        }
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