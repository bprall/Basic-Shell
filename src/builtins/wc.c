#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_ARGS 32

int *get_counts(char *filename) {
    int *counts = malloc(3 * sizeof(int));
    if (!counts) {
        perror("Memory allocation failed");
        return NULL;
    }

    int fd;
    if (strcmp(filename, "") == 0) {
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror(filename);
            free(counts);
            return NULL;
        }
    }

    int in_whitespace = 1;
    counts[0] = 0; // Lines
    counts[1] = 0; // Words
    counts[2] = 0; // Characters
    char c;

    while (read(fd, &c, 1) > 0) {
        counts[2]++;
        if (c == '\n') {
            counts[0]++;
            in_whitespace = 1;
        } else if (isspace(c)) {
            if (!in_whitespace) {
                counts[1]++;
                in_whitespace = 1;
            }
        } else {
            in_whitespace = 0;
        }
    }
    if (fd != STDIN_FILENO) {
        close(fd);
    }
    return counts;
}

void print_counts(int *show, int *count, char *name) {
    if (show[0]) printf("%8d ", count[0]);
    if (show[1]) printf("%8d ", count[1]);
    if (show[2]) printf("%8d ", count[2]);
    printf("%s\n", name);
}