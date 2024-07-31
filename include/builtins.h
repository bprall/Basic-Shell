#ifndef BUILDINS_H
#define BUILDINS_H

void cmd_help(const char *command);
int *get_counts(char *filename);
void print_counts(int *show, int *count, char *name);
char *strappend_str(char *s, char *t);
char *strappend_char(char *s, char c);
unsigned int find_encoding(char *dictionary[], char *s);
void write_code(int fd, char *dictionary[], char *s);
void compress(char *in_file_name, char *out_file_name);
unsigned int read_code(int fd);
void uncompress(char *in_file_name, char *out_file_name);

#endif // BUILDINS_H
