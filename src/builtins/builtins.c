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
#include "../../include/builtins/zip.h"
#include "../../include/builtins/wc.h"
#include "../../include/builtins/sortwords.h"
#include "../../include/builtins/alias.h"


int execute_builtin_commands(char **command_line_words, size_t num_args) {
    int result;
    
    if (strcmp(command_line_words[0], "wc") == 0) {
        result = execute_wc_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "cd") == 0) {
        if (num_args > 1) {
            if (chdir(command_line_words[1]) != 0) {
                perror("chdir");
                result = 0;
            } else {
                result = 1;
            }
        } else {
            printf("cd: missing argument\n");
            result = 0;
        }
    } else if (strcmp(command_line_words[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
            result = 1;
        } else {
            perror("getcwd");
            result = 0;
        }
    } else if (strcmp(command_line_words[0], "help") == 0) {
        result = execute_help_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "sort") == 0) {
        result = execute_sort_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "zip") == 0) {
        result = execute_zip_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "unzip") == 0) {
        result = execute_zip_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "sortwords") == 0) {
        result = execute_sortwords_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "alias") == 0) {
        result = execute_alias_command(command_line_words, num_args);
    } else if (strcmp(command_line_words[0], "unalias") == 0) {
        result = remove_alias(command_line_words[1]);
    } else {
        result = -1;
    }
    
    return result;
}