#ifndef BUILTINS_H
#define BUILTINS_H

void cmd_help(const char *command);
void sort(double *a, int length);
int *get_counts(char *filename);
void compress(char *in_file_name, char *out_file_name);
void uncompress(char *in_file_name, char *out_file_name);

#endif // BUILTINS_H
