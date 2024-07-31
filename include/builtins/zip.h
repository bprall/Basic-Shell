#ifndef ZIP_H
#define ZIP_H

unsigned int find_encoding(char *dictionary[], char *s);
void write_code(int fd, char *dictionary[], char *s);
void compress(char *in_file_name, char *out_file_name);

#endif // ZIP_H