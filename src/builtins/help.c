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
        printf("\tsortwords [input.txt] [output.txt] - Sort words in a file and write to another file\n");
        printf("\tmv [source] [destination] - Move or rename files and directories\n");
        printf("\techo [text] - Print the specified text to the terminal\n");
        printf("\tcat [file...] - Concatenate and display the content of files\n");
        printf("\tgrep [pattern] [file...] - Search for a specified pattern in files and print matching lines\n");
        printf("\tclear - Clear the terminal screen\n");
        printf("\talias [alias_name='command'] - Create an alias for a command, or list aliases if no 'aliasname' given.\n");
        printf("\tunalias [alias_name] - Remove an alias\n");
        printf("\texit - Exit the shell\n");
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
    } else if (strcmp(command, "sortwords") == 0) {
        // Display help for the sortwords command
        printf("Usage: sortwords [input.txt] [output.txt]\n");
        printf("Description: Sorts words in a file in alphabetical order and writes the result to another file, or overwrite the input file if no output file is specified..\n");
        printf("Example:\n");
        printf("\tcssh$ sortwords input.txt output.txt\n");
    } else if (strcmp(command, "wc") == 0) {
        // Display help for the wc command
        printf("Usage: wc [-l] [-w] [-c] [FILES...]\n");
        printf("Description: Prints the number of lines, words, and characters in files. If no files are given, reads from standard input.\n");
        printf("Options:\n");
        printf("\t-l\tPrint the number of lines\n");
        printf("\t-w\tPrint the number of words\n");
        printf("\t-c\tPrint the number of characters\n");
        printf("Example:\n");
        printf("\tcssh$ wc -l file.txt\n");
    } else if (strcmp(command, "mv") == 0) {
        // Display help for the mv command
        printf("Usage: mv [source] [destination]\n");
        printf("Description: Moves or renames files and directories.\n");
        printf("Example:\n");
        printf("\tcssh$ mv file1.txt /new/location/file1.txt\n");
        printf("\tcssh$ mv oldname.txt newname.txt\n");
    } else if (strcmp(command, "echo") == 0) {
        // Display help for the echo command
        printf("Usage: echo [text]\n");
        printf("Description: Prints the specified text to the terminal.\n");
        printf("Example:\n");
        printf("\tcssh$ echo \"Hello, World!\"\n");
    } else if (strcmp(command, "cat") == 0) {
        // Display help for the cat command
        printf("Usage: cat [file...]\n");
        printf("Description: Concatenates and displays the content of files.\n");
        printf("Example:\n");
        printf("\tcssh$ cat file1.txt file2.txt\n");
    } else if (strcmp(command, "grep") == 0) {
        // Display help for the grep command
        printf("Usage: grep [pattern] [file...]\n");
        printf("Description: Searches for a specified pattern in files and prints matching lines.\n");
        printf("Example:\n");
        printf("\tcssh$ grep 'pattern' file.txt\n");
    } else if (strcmp(command, "clear") == 0) {
        // Display help for the clear command
        printf("Usage: clear\n");
        printf("Description: Clears the terminal screen.\n");
    } else if (strcmp(command, "alias") == 0) {
        // Display help for the alias command
        printf("Usage: alias aliasname=\"command\"\n");
        printf("Description: Creates an alias for a command. The alias will substitute the command with the provided name.\n");
        printf("Example:\n");
        printf("\tcssh$ alias ll='ls -l'\n");
    } else if (strcmp(command, "unalias") == 0) {
        // Display help for the unalias command
        printf("Usage: unalias [alias_name]\n");
        printf("Description: Removes an existing alias.\n");
        printf("Example:\n");
        printf("\tcssh$ unalias ll\n");
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
