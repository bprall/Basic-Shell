#ifndef UTILS_H
#define UTILS_H

void free_command(char **words);
void sigchld_handler(int signo);
char *strappend_str(char *s, char *t);
char *strappend_char(char *s, char c);

#endif // UTILS_H