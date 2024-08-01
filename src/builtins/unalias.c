#include "../../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/builtins/alias.h"

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