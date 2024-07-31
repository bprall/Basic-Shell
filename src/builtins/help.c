#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
        printf("\tcssh$ sort 3.6 1 4 1.1 5 9 2.7 6 5.3 3 5\n");
        printf("\tcssh$ sort -r 3.6 1 4 1.1 5 9 2.7 6 5.3 3 5\n");
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