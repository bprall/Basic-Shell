#include "../include/config.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../include/utils.h"
#include "../../include/builtins/help.h"
#include "../../include/builtins/sort.h"
#include "../../include/builtins/unzip.h"
#include "../../include/builtins/zip.h"
#include "../../include/builtins/wc.h"
#include "../../include/builtins/sortwords.h"
#include "../../include/builtins/alias.h"

int execute_zip_commands(char **command_line_words, size_t num_args) {
    if (strcmp(command_line_words[0], "zip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            compress(input_file, output_file);
            return 1;
        } else {
            printf("Usage: zip [input_file] [output_file]\n");
            return 0;
        }
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        if (num_args == 3) {
            char *input_file = command_line_words[1];
            char *output_file = command_line_words[2];
            uncompress(input_file, output_file);
            return 1;
        } else {
            printf("Usage: unzip [input_file] poutput_file]\n");
            return 0;
        }
    }
    return 0;
}

int execute_help_command(char **command_line_words, size_t num_args) {
    if (num_args > 1) {
        cmd_help(command_line_words[1]);
    } else {
        cmd_help(NULL);
    }
    return 1; 
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

int execute_sortwords_command(char **command_line_words, size_t num_args) {
    if (num_args < 2 || num_args > 3) {
        fprintf(stderr, "Usage: sortwords [input_file] [output_file]\n");
        return 0;
    }

    const char *input_file = command_line_words[1];
    const char *output_file = (num_args == 3) ? command_line_words[2] : input_file;

    return sortwords(input_file, output_file);
}

int execute_grep_command(char **command_line_words, size_t num_args) {
    if (num_args < 2) {
        printf("Usage: grep <pattern> [file...]\n");
        return 0;
    }

    char *pattern = command_line_words[1];
    int found = 0;
    
    if (num_args > 2) {
        for (size_t i = 2; i < num_args; ++i) {
            FILE *file = fopen(command_line_words[i], "r");
            if (file == NULL) {
                perror("fopen");
                continue;
            }
            
            char line[MAX_LINE_LENGTH];
            while (fgets(line, sizeof(line), file) != NULL) {
                if (strstr(line, pattern) != NULL) {
                    printf("%s: %s", command_line_words[i], line);
                    found = 1;
                }
            }
            fclose(file);
        }
    } else {
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (strstr(line, pattern) != NULL) {
                printf("%s", line);
                found = 1;
            }
        }
    }

    return found ? 1 : 0;
}

int execute_alias_command(char **command_line_words, size_t num_args) {
    if (num_args < 2) {
        print_aliases();
        return 1;
    }

    char *alias_command = concatenate_args(command_line_words, num_args);
    if (!alias_command) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }

    char alias[MAX_ALIAS_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    char *equal_sign;

    equal_sign = strchr(alias_command, '=');
    if (equal_sign == NULL) {
        fprintf(stderr, "Invalid Alias format.\nUsage: alias aliasname=\"command\"\n");
        free(alias_command);
        return 0;
    }

    size_t alias_length = equal_sign - alias_command;
    if (alias_length >= MAX_ALIAS_LENGTH) {
        fprintf(stderr, "Alias name too long\n");
        free(alias_command);
        return 0;
    }
    strncpy(alias, alias_command, alias_length);
    alias[alias_length] = '\0';

    char *command_start = equal_sign + 1;
    while (*command_start == ' ') command_start++;
    char *command_end = command_start + strlen(command_start) - 1;
    while (command_end > command_start && *command_end == ' ') command_end--;
    command_end++;

    size_t command_length = command_end - command_start;
    if (command_length >= MAX_COMMAND_LENGTH) {
        fprintf(stderr, "Command too long\n");
        free(alias_command);
        return 0;
    }
    strncpy(command, command_start, command_length);
    command[command_length] = '\0';

    remove_quotes(command);

    if (!add_alias(alias, command)) {
        fprintf(stderr, "Failed to add alias\n");
        free(alias_command);
        return 0;
    }

    free(alias_command);
    return 1;
}