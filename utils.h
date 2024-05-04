#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);

int shell_num_builtins();

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

#endif // UTILS_H