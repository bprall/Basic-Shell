#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../include/utils.h"

#define NUM_CODES 65536


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

void compress(char *in_file_name, char *out_file_name)
{
    FILE *input_file = fopen(in_file_name, "r");
    if (input_file == NULL)
    {
        perror("Error opening input file");
        exit(1);
    }

    int output_fd = open(out_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        perror("Error creating output file");
        exit(1);
    }

    char *dictionary[NUM_CODES] = {0};
    char *currentString = NULL;
    char currentChar;

    if (fscanf(input_file, "%c", &currentChar) != 1)
    {
        fclose(input_file);
        close(output_fd);
        return;
    }

    currentString = strappend_char(currentString, currentChar);

    while (fscanf(input_file, "%c", &currentChar) == 1) {
        char *tempString = strappend_char(currentString, currentChar);
        unsigned int code = find_encoding(dictionary, tempString);

        if (code != NUM_CODES) {
            free(currentString);
            currentString = tempString;
        } else {
            write_code(output_fd, dictionary, currentString);

            if (code < NUM_CODES) {
                dictionary[code] = strappend_char(currentString, currentChar);
            }

            free(currentString);
            currentString = strappend_char(NULL, currentChar);
        }
        free(tempString);
    }


    write_code(output_fd, dictionary, currentString);

    free(currentString);
    for (int i = 0; i < NUM_CODES; i++)
    {
        if (dictionary[i] != NULL)
        {
            free(dictionary[i]);
        }
        else
        {
            break;
        }
    }
    fclose(input_file);
    close(output_fd);
}
