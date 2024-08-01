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