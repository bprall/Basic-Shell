#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_ARGS 32
#define NUM_CODES 65536


void free_command(char **words) {
    for (int i = 0; i < MAX_ARGS; ++i) {
        if (words[i] == NULL) {
            break;
        }
        free(words[i]);
    }
    free(words);
}

void sigchld_handler(int signo) {
    (void) signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

void print_counts(int *show, int *count, char *name) {
    if (show[0]) printf("%8d ", count[0]);
    if (show[1]) printf("%8d ", count[1]);
    if (show[2]) printf("%8d ", count[2]);
    printf("%s\n", name);
}

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

char *strappend_str(char *s, char *t)
{
    if (s == NULL || t == NULL)
    {
        return NULL;
    }

    int new_size = strlen(s) + strlen(t) + 1;
    char *result = (char *)malloc(new_size*sizeof(char));
    strcpy(result, s);
    strcat(result, t);

    return result;
}

char *strappend_char(char *s, char c)
{
    if (s == NULL)
    {
        return NULL;
    }

    int new_size = strlen(s) + 2;
    char *result = (char *)malloc(new_size*sizeof(char));
    strcpy(result, s);
    result[new_size-2] = c;
    result[new_size-1] = '\0';

    return result;
}

unsigned int find_encoding(char *dictionary[], char *s)
{
    if (dictionary == NULL || s == NULL)
    {
        return NUM_CODES;
    }
    for (unsigned int i=0; i<NUM_CODES; ++i)
    {
        if (dictionary[i] == NULL)
        {
            break;
        }

        if (strcmp(dictionary[i], s) == 0)
        {
            return i;
        }
    }
    return NUM_CODES;
}

void write_code(int fd, char *dictionary[], char *s)
{
    if (dictionary == NULL || s == NULL)
    {
        return;
    }

    unsigned int code = find_encoding(dictionary, s);
    if (code == NUM_CODES)
    {
        printf("Algorithm error!");
        exit(1);
    }

    unsigned short actual_code = (unsigned short)code;
    if (write(fd, &actual_code, sizeof(unsigned short)) != sizeof(unsigned short))
    {
        perror("write");
        exit(1);
    }
}

unsigned int read_code(int fd)
{
    unsigned short actual_code;
    int read_return = read(fd, &actual_code, sizeof(unsigned short));
    if (read_return == 0)
    {
        return NUM_CODES;
    }
    if (read_return != sizeof(unsigned short))
    {
       perror("read");
       exit(1);
    }
    return (unsigned int)actual_code;
}