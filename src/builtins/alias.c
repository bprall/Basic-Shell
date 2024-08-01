#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../../include/builtins/alias.h"
#include "../../include/config.h"

AliasTable alias_table = {.mutex = PTHREAD_MUTEX_INITIALIZER};

unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash % TABLE_SIZE;
}

int add_alias(const char *alias, const char *command) {
    pthread_mutex_lock(&alias_table.mutex);

    unsigned int index = hash(alias);
    AliasEntry *entry = alias_table.table[index];
    while (entry != NULL) {
        if (strcmp(entry->alias, alias) == 0) {
            strncpy(entry->command, command, MAX_COMMAND_LENGTH - 1);
            entry->command[MAX_COMMAND_LENGTH - 1] = '\0';
            pthread_mutex_unlock(&alias_table.mutex);
            return 1;
        }
        entry = entry->next;
    }

    entry = (AliasEntry *)malloc(sizeof(AliasEntry));
    if (!entry) {
        perror("malloc");
        pthread_mutex_unlock(&alias_table.mutex);
        return 0;
    }

    strncpy(entry->alias, alias, MAX_ALIAS_LENGTH - 1);
    entry->alias[MAX_ALIAS_LENGTH - 1] = '\0';
    strncpy(entry->command, command, MAX_COMMAND_LENGTH - 1);
    entry->command[MAX_COMMAND_LENGTH - 1] = '\0';
    entry->next = alias_table.table[index];
    alias_table.table[index] = entry;

    pthread_mutex_unlock(&alias_table.mutex);
    return 1;
}

const char* get_command_for_alias(const char *alias) {
    pthread_mutex_lock(&alias_table.mutex);

    unsigned int index = hash(alias);
    AliasEntry *entry = alias_table.table[index];
    while (entry != NULL) {
        if (strcmp(entry->alias, alias) == 0) {
            pthread_mutex_unlock(&alias_table.mutex);
            return entry->command;
        }
        entry = entry->next;
    }

    pthread_mutex_unlock(&alias_table.mutex);
    return NULL;
}

void remove_quotes(char *str) {
    size_t len = strlen(str);

    if (len > 0 && str[0] == '"' && str[len - 1] == '"') {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
    else if (len > 0 && str[0] == '\'' && str[len - 1] == '\'') {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void print_aliases() {
    pthread_mutex_lock(&alias_table.mutex);

    printf("Aliases:\n");
    for (unsigned int i = 0; i < TABLE_SIZE; ++i) {
        AliasEntry *entry = alias_table.table[i];
        while (entry != NULL) {
            printf("Alias: %s\tCommand: %s\n\n", entry->alias, entry->command);
            entry = entry->next;
        }
    }

    pthread_mutex_unlock(&alias_table.mutex);
}

char* concatenate_args(char **command_line_words, size_t num_args) {
    if (num_args < 2) return NULL; 

    size_t total_length = 0;
    for (size_t i = 1; i < num_args; ++i) {
        total_length += strlen(command_line_words[i]) + 1;
    }

    char *command = (char *)malloc(total_length + 1);
    if (!command) {
        perror("malloc");
        return NULL;
    }

    command[0] = '\0';
    for (size_t i = 1; i < num_args; ++i) {
        strcat(command, command_line_words[i]);
        if (i < num_args - 1) {
            strcat(command, " "); 
        }
    }

    return command;
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

int remove_alias(const char *alias) {
    pthread_mutex_lock(&alias_table.mutex);

    unsigned int index = hash(alias);
    AliasEntry *entry = alias_table.table[index];
    AliasEntry *prev = NULL;

    while (entry != NULL) {
        if (strcmp(entry->alias, alias) == 0) {
            if (prev == NULL) {
                alias_table.table[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry);
            pthread_mutex_unlock(&alias_table.mutex);
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }

    pthread_mutex_unlock(&alias_table.mutex);
    return 0;
}