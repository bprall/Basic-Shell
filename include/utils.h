#ifndef UTILS_H
#define UTILS_H

void free_command(char **words);
void sigchld_handler(int signo);
void print_counts(int *show, int *count, char *name);
int is_valid_num(char *word);
void swap(double *x, double *y);
char *strappend_str(char *s, char *t);
char *strappend_char(char *s, char c);
unsigned int find_encoding(char *dictionary[], char *s);
void write_code(int fd, char *dictionary[], char *s);
unsigned int read_code(int fd);

#endif // UTILS_H