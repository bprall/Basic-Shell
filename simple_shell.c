#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_ARGS 32

char **get_next_command(size_t *num_args)
{
    printf("cssh$ ");

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    if (ferror(stdin))
    {
        perror("getline");
        exit(1);
    }
    if (feof(stdin))
    {
        return NULL;
    }

    char **words = (char **)malloc(MAX_ARGS*sizeof(char *));
    int i=0;

    char *parse = line;
    while (parse != NULL)
    {
        char *word = strsep(&parse, " \t\r\f\n");
        if (strlen(word) != 0)
        {
            words[i++] = strdup(word);
        }
    }
    *num_args = i;
    for (; i<MAX_ARGS; ++i)
    {
        words[i] = NULL;
    }

    free(line);

    return words;
}

void free_command(char **words)
{
    for (int i=0; i<MAX_ARGS; ++i)
    {
        if (words[i] == NULL)
        {
            break;
        }
        free(words[i]);
    }
    free(words);
}

int main()
{
    size_t num_args;

    char **command_line_words = get_next_command(&num_args);
    while (command_line_words != NULL)
    {
        if (command_line_words[0] == NULL) {
            free_command(command_line_words);
            command_line_words = get_next_command(&num_args);
            continue;
        }

        if (strcmp(command_line_words[0], "exit") == 0) {
            free_command(command_line_words);
            break;
        }

        int input_redirection = 0;
        int output_redirection = 0;
        int append_redirection = 0;
        char *input_file = NULL;
        char *output_file = NULL;

        for (int i = 0; command_line_words[i] != NULL; i++) {
            if (strcmp(command_line_words[i], "<") == 0) {
                if (input_redirection) {
                    printf("Error! Can't have two <'s!\n");
                    free_command(command_line_words);
                    command_line_words = get_next_command(&num_args);
                    break;
                }
                input_redirection = 1;
                input_file = command_line_words[i + 1];
                command_line_words[i] = NULL;
            } else if (strcmp(command_line_words[i], ">") == 0) {
                if (output_redirection || append_redirection) {
                    printf("Error! Can't have two >'s or >>'s!\n");
                    free_command(command_line_words);
                    command_line_words = get_next_command(&num_args);
                    break;
                }
                output_redirection = 1;
                output_file = command_line_words[i + 1];
                command_line_words[i] = NULL;
            } else if (strcmp(command_line_words[i], ">>") == 0) {
                if (output_redirection || append_redirection) {
                    printf("Error! Can't have two >'s or >>'s!\n");
                    free_command(command_line_words);
                    command_line_words = get_next_command(&num_args);
                    break;
                }
                append_redirection = 1;
                output_file = command_line_words[i + 1];
                command_line_words[i] = NULL;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            if (input_redirection) {
                int fd = open(input_file, O_RDONLY);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(1);
                }
                close(fd);
            }
            if (output_redirection) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(1);
                }
                close(fd);
            }
            if (append_redirection) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(1);
                }
                close(fd);
            }

            execvp(command_line_words[0], command_line_words);
            perror("execvp");
            exit(1);
        } else {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
                exit(1);
            }
        }

        free_command(command_line_words);

        command_line_words = get_next_command(&num_args);
    }

    free_command(command_line_words);

    return 0;
}