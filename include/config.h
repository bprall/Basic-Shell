#ifndef CONFIG_H
#define CONFIG_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L  
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE         
#endif

#ifndef _BSD_SOURCE
#define _BSD_SOURCE             
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef MAX_COMMAND_LENGTH
#define MAX_COMMAND_LENGTH 1024
#endif

#define MAX_ARGS 32
#define MAX_LINE_LENGTH 1024
#define MAX_WORDS 10000

#endif // CONFIG_H
