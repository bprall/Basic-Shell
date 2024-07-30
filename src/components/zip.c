#define _POSIX_C_SOURCE 200809L // required for strdup() on cslab

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../include/zip_utils.h"

/* Use 16-bit code words */
#define NUM_CODES 65536

/* allocate space for and return a new string s+t */
char *strappend_str(char *s, char *t);

/* look for string s in the dictionary
 * return the code if found
 * return NUM_CODES if not found 
 */
unsigned int find_encoding(char *dictionary[], char *s);

/* write the code for string s to file */
void write_code(int fd, char *dictionary[], char *s);

/* compress in_file_name to out_file_name */
void compress(char *in_file_name, char *out_file_name);


// Comment out to remove this function
/*
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: zip file\n");
        exit(1);
    }

    char *in_file_name = argv[1];
    char *out_file_name = strappend_str(in_file_name, ".zip");

    compress(in_file_name, out_file_name);

    // have to free the memory for out_file_name since strappend_str malloc()'ed it
    free(out_file_name);

    return 0;
}
*/

/* look for string s in the dictionary
 * return the code if found
 * return NUM_CODES if not found 
 */
unsigned int find_encoding(char *dictionary[], char *s)
{
    if (dictionary == NULL || s == NULL)
    {
        return NUM_CODES;
    }

    for (unsigned int i=0; i<NUM_CODES; ++i)
    {
        /* code words are added in order, so if we get to a NULL value 
         * we can stop searching */
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

/* write the code for string s to file */
void write_code(int fd, char *dictionary[], char *s)
{
    if (dictionary == NULL || s == NULL)
    {
        return;
    }

    unsigned int code = find_encoding(dictionary, s);
    // should never call write_code() unless s is in the dictionary 
    if (code == NUM_CODES)
    {
        printf("Algorithm error!");
        exit(1);
    }

    // cast the code to an unsigned short to only use 16 bits per code word in the output file
    unsigned short actual_code = (unsigned short)code;
    if (write(fd, &actual_code, sizeof(unsigned short)) != sizeof(unsigned short))
    {
        perror("write");
        exit(1);
    }
}

/* compress in_file_name to out_file_name */
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
            currentString = tempString; // Safe to use tempString here
        } else {
            write_code(output_fd, dictionary, currentString);

            if (code < NUM_CODES) {
                dictionary[code] = strappend_char(currentString, currentChar);
            }

            free(currentString);
            currentString = strappend_char(NULL, currentChar);
        }
        free(tempString); // Free tempString here to avoid memory leaks
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