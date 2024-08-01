#ifndef WC_H
#define WC_H

int *get_counts(char *filename);
void print_counts(int *show, int *count, char *name);
int execute_wc_command(char **command_line_words, size_t num_args);


#endif // WC_H