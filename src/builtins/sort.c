#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int is_valid_num(char *word) {
    int found_dot = 0;

    for (int i = 0; i < strlen(word); ++i) {
        if (word[i] == '-') {
            if (i != 0) {
                return 0;
            }
            continue;
        }

        if (word[i] == '.') {
            if (found_dot == 1) {
                return 0;
            }
            found_dot = 1;
            continue;
        }

        if (!isdigit(word[i])) {
            return 0;
        }
    }

    return 1;
}

void swap(double *x, double *y) {
    if (x != NULL && y != NULL) {
        double temp = *x;
        *x = *y;
        *y = temp;
    }
}

void sort(double *a, int length) {
    if (a == NULL) return;

    for (int i = 1; i < length; i++) {
        int j = i;
        while (j > 0 && a[j - 1] > a[j]) {
            swap(&a[j], &a[j - 1]);
            j--;
        }
    }
}

int execute_sort_command(char **command_line_words, size_t num_args) {
    if (num_args < 2) {
        return 0; 
    }

    int reverse = 0;
    int actual_num_args = num_args - 1;

    if (strcmp(command_line_words[1], "-r") == 0) {
        reverse = 1;
        if (actual_num_args < 2) {
            return 0; 
        }
        actual_num_args--;
        command_line_words++;
    }

    double *numbers = (double *)malloc(actual_num_args * sizeof(double));
    if (numbers == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }

    for (int i = 0; i < actual_num_args; i++) {
        if (!is_valid_num(command_line_words[i + 1])) {
            fprintf(stderr, "%s is not a valid number.\n", command_line_words[i + 1]);
            free(numbers);
            return 0;
        }
        numbers[i] = atof(command_line_words[i + 1]);
    }

    sort(numbers, actual_num_args);

    if (reverse) {
        for (int i = actual_num_args - 1; i >= 0; i--) {
            printf("%g\n", numbers[i]);
        }
    } else {
        for (int i = 0; i < actual_num_args; i++) {
            printf("%g\n", numbers[i]);
        }
    }

    free(numbers);
    return 1;
}