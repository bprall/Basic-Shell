#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>


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

int execute_wc_command(char **command_line_words, size_t num_args) {
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
                return 0;
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
            } else {
                return 0;
            }
        }
    }

    if (file_count == 0) {
        int *counts = get_counts("");
        if (counts != NULL) {
            print_counts(show, counts, "");
            free(counts);
        } else {
            return 0;
        }
    } else if (file_count > 1) {
        print_counts(show, total_counts, "total");
    }
    return 1;
}