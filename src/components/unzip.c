#define _POSIX_C_SOURCE 200809L

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


/* read the next code from fd
 * return NUM_CODES on EOF
 * return the code read otherwise
 */
unsigned int read_code(int fd);

/* uncompress in_file_name to out_file_name */
void uncompress(char *in_file_name, char *out_file_name);

// Comment out or remove this function
/*
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: unzip file\n");
        exit(1);
    }

    char *in_file_name = argv[1];
    char *out_file_name = strdup(in_file_name);
    out_file_name[strlen(in_file_name)-4] = '\0';

    uncompress(in_file_name, out_file_name);

    free(out_file_name);

    return 0;
}
*/

/* read the next code from fd
 * return NUM_CODES on EOF
 * return the code read otherwise
 */
unsigned int read_code(int fd)
{
    // code words are 16-bit unsigned shorts in the file
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

/* uncompress in_file_name to out_file_name */
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