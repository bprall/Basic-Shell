#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_ARGS 32
#define NUM_CODES 65536

void cmd_help(const char *command) {
    if (command == NULL) {
        // No command specified, display general help
        printf("Available commands:\n");
        printf("\thelp [command] - Display usage information for a command\n");
        printf("\thistory - Show command history\n");
        printf("\tzip [file] [output_zip] - Compress a file into a zip archive\n");
        printf("\tunzip [archive] [output_file] - Decompress a zip archive into a file\n");
        printf("\tcd [directory] - Change the current directory\n");
        printf("\tpwd - Print the current working directory\n");
        printf("\twc [-l] [-w] [-c] [FILES...] - Print the number of lines, words, and characters in files\n");
        printf("\tsort [-r] num1 num2 ... - Sort numbers in ascending or descending order\n");
        printf("\texit - Exit the shell\n");
        // Other commands 
    } else if (strcmp(command, "history") == 0) {
        // Display help for the history command
        printf("Usage: history [count]\n");
        printf("Description: Displays the command history. Optionally, specify the number of recent commands to show.\n");
        printf("Example:\n");
        printf("\tcssh$ history 10\n");
    } else if (strcmp(command, "zip") == 0) {
        // Display help for the zip command
        printf("Usage: zip [file] [output_zip]\n");
        printf("Description: Compresses a file into a zip archive. If the output_zip is not specified, the output file will have the same name as the input file with a .zip extension.\n");
        printf("Example:\n");
        printf("\tcssh$ zip example.txt example.zip\n");
        printf("\tcssh$ zip example.txt\n");
    } else if (strcmp(command, "unzip") == 0) {
        // Display help for the unzip command
        printf("Usage: unzip [archive] [output_file]\n");
        printf("Description: Decompresses a zip archive into a file. If the output_file is not specified, the file will have the same name as the archive.\n");
        printf("Example:\n");
        printf("\tcssh$ unzip example.zip example.txt\n");
        printf("\tcssh$ unzip example.zip\n");
    } else if (strcmp(command, "cd") == 0) {
        // Display help for the cd command
        printf("Usage: cd [directory]\n");
        printf("Description: Changes the current directory to the specified directory.\n");
        printf("Example:\n");
        printf("\tcssh$ cd /path/to/directory\n");
    } else if (strcmp(command, "pwd") == 0) {
        // Display help for the pwd command
        printf("Usage: pwd\n");
        printf("Description: Prints the current working directory.\n");
        printf("Example:\n");
        printf("\tcssh$ pwd\n");
    } else if (strcmp(command, "sort") == 0) {
        // Display help for the sort command
        printf("Usage: sort [-r] num1 num2 ...\n");
        printf("Description: Sorts a list of numbers in ascending order by default. If '-r' is specified, sorts in descending order.\n");
        printf("Options:\n");
        printf("\t-r\tReverse the order (sort in descending order)\n");
        printf("Example:\n");
        printf("\tcssh$ sort 3 1 4 1 5 9 2 6 5 3 5\n");
        printf("\tcssh$ sort -r 3 1 4 1 5 9 2 6 5 3 5\n");
    } else if (strcmp(command, "wc") == 0) {
        // Display help for the wc command
        printf("Usage: wc [-l] [-w] [-c] [FILES...]\n");
        printf("where:\n");
        printf("\t-l prints the number of lines\n");
        printf("\t-w prints the number of words\n");
        printf("\t-c prints the number of characters\n");
        printf("\tFILES if no files are given, then read from standard input\n");
    } else if (strcmp(command, "exit") == 0) {
        // Display help for the exit command
        printf("Usage: exit\n");
        printf("Description: Exits the shell.\n");
        printf("Example:\n");
        printf("\tcssh$ exit\n");
    } else {
        printf("No help available for '%s'.\n", command);
    }
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

void sort(double *a, int length) {
    if (a == NULL) return;

    for (int i = 1; i < length; i++) {
        int j = i;
        while (j > 0 && a[j - 1] > a[j]) {
            swap(&a[j], &a[j - 1]);
            j--;
        }
    }
}

int *get_counts(char *filename) {
    int *counts = malloc(3 * sizeof(int));
    if (!counts) {
        perror("Memory allocation failed");
        return NULL;
    }

    int fd;
    if (strcmp(filename, "") == 0) {
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror(filename);
            free(counts);
            return NULL;
        }
    }

    int in_whitespace = 1;
    counts[0] = 0; // Lines
    counts[1] = 0; // Words
    counts[2] = 0; // Characters
    char c;

    while (read(fd, &c, 1) > 0) {
        counts[2]++;
        if (c == '\n') {
            counts[0]++;
            in_whitespace = 1;
        } else if (isspace(c)) {
            if (!in_whitespace) {
                counts[1]++;
                in_whitespace = 1;
            }
        } else {
            in_whitespace = 0;
        }
    }
    if (fd != STDIN_FILENO) {
        close(fd);
    }
    return counts;
}

void print_counts(int *show, int *count, char *name) {
    if (show[0]) printf("%8d ", count[0]);
    if (show[1]) printf("%8d ", count[1]);
    if (show[2]) printf("%8d ", count[2]);
    printf("%s\n", name);
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