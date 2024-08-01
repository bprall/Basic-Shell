#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/utils.h"
#include "../../include/config.h"

int compare_words(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

size_t read_words(const char *filename, char *words[], size_t max_words) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    size_t word_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char *word = strtok(line, " \t\n");
        while (word != NULL) {
            if (word_count >= max_words) {
                fprintf(stderr, "Exceeded maximum number of words\n");
                fclose(file);
                return word_count;
            }

            words[word_count] = strdup(word);
            if (words[word_count] == NULL) {
                perror("Failed to allocate memory");
                fclose(file);
                return word_count;
            }
            word_count++;

            word = strtok(NULL, " \t\n");
        }
    }

    fclose(file);
    return word_count;
}

void free_words(char *words[], size_t word_count) {
    for (size_t i = 0; i < word_count; i++) {
        free(words[i]);
    }
}

int sortwords(const char *input_file, const char *output_file) {
    char *words[MAX_WORDS];
    size_t word_count = read_words(input_file, words, MAX_WORDS);
    
    if (word_count == 0) {
        return 0;
    }

    merge_sort(words, 0, word_count - 1);

    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {
        perror("Failed to open output file");
        free_words(words, word_count);
        return 0;
    }

    for (size_t i = 0; i < word_count; i++) {
        fprintf(outfile, "%s\n", words[i]);
    }

    fclose(outfile);
    free_words(words, word_count);

    return 1;
}