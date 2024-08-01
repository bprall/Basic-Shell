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

void uncompress(char *in_file_name, char *out_file_name)
{
    char *Dictionary[NUM_CODES];
    int nextIndex = 256;

    for (int i = 0; i < 256; i++) {
        Dictionary[i] = (char *)malloc(2 * sizeof(char));
        Dictionary[i][0] = (char)i;
        Dictionary[i][1] = '\0';
    }

    int in_fd = open(in_file_name, O_RDONLY);
    if (in_fd == -1) {
        perror("open");
        exit(1);
    }

    int out_fd = open(out_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out_fd == -1) {
        perror("open");
        exit(1);
    }

    unsigned int CurrentCode = read_code(in_fd);
    char *CurrentChar = "";
    write(out_fd, CurrentChar, strlen(CurrentChar));

    unsigned int NextCode;
    char *CurrentString;
    char *OldString;
    char nextChar;
    while ((NextCode = read_code(in_fd)) != NUM_CODES) {
        if (NextCode >= NUM_CODES) {
            fprintf(stderr, "Invalid code: %u\n", NextCode);
            exit(1);
        }

        if (Dictionary[NextCode] == NULL) {
            CurrentString = strappend_char(Dictionary[CurrentCode], CurrentChar[0]);
        } else {
            CurrentString = Dictionary[NextCode];
        }

        write(out_fd, CurrentString, strlen(CurrentString));

        nextChar = CurrentString[0];
        OldString = Dictionary[CurrentCode];
        Dictionary[nextIndex++] = strappend_char(OldString, nextChar);

        CurrentCode = NextCode;
        CurrentChar = Dictionary[CurrentCode];
    }

    close(in_fd);
    close(out_fd);

    for (int i = 0; i < nextIndex; i++) {
        free(Dictionary[i]);
    }
}

int execute_zip_command(char **command_line_words, size_t num_args) {
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