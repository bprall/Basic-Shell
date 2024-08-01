#ifndef ZIP_H
#define ZIP_H

char *strappend_str(char *s, char *t);
char *strappend_char(char *s, char c);
unsigned int find_encoding(char *dictionary[], char *s);
unsigned int read_code(int fd);
void write_code(int fd, char *dictionary[], char *s);
void compress(char *in_file_name, char *out_file_name);
void uncompress(char *in_file_name, char *out_file_name);
int execute_zip_command(char **command_line_words, size_t num_args);


#endif // ZIP_H