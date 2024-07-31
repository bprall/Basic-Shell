#ifndef UNZIP_H
#define UNZIP_H

unsigned int read_code(int fd);
void uncompress(char *in_file_name, char *out_file_name);

#endif // UNZIP_H