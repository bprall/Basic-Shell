#ifndef ALIAS_H
#define ALIAS_H

#include <pthread.h>
#include <stddef.h>

#define TABLE_SIZE 100
#define MAX_ALIAS_LENGTH 256
#define MAX_COMMAND_LENGTH 1024

typedef struct AliasEntry {
    char alias[MAX_ALIAS_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    struct AliasEntry *next;
} AliasEntry;

typedef struct {
    AliasEntry *table[TABLE_SIZE];
    pthread_mutex_t mutex;
} AliasTable;

extern AliasTable alias_table;

unsigned int hash(const char *str);
int add_alias(const char *alias, const char *command);
const char* get_command_for_alias(const char *alias);
void remove_quotes(char *str);
void print_aliases();

#endif // ALIAS_H
