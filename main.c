#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/************************************************/

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

int shell_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int shell_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }
  }
  return 1;
}

int shell_help(char **args) {
  int i;
  printf("Shell implementation\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < shell_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int shell_exit(char **args){
  return 0;
}

/*************************************************/

int shell_launch(char** args) {
    pid_t pid;
    pid_t wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        // child process
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // error forking
        perror("shell");
    } else {
        // parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFEXITED(status));
    }
    return 1;
}

int shell_execute(char** args) {
    int i;

    if (args[0] == NULL) {
        // an empty command was entered.
        return 1;
    }

    for (i = 0; i < shell_num_builtins(); ++i) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    fprintf(stderr, "shell: command not found: %s\n", args[0]);
    return 1;
}

char* shell_read_line(void) {

#define SHELL_BUFFER_SIZE 1024

    int buffer_size = SHELL_BUFFER_SIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer) {
        fprintf(stderr, "shell: allocation error!\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // read a character
        c = getchar();

        // if we hit EOF, replace it with a null character and return
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        ++position;

        // if we have exceeded the buffer, reallocate.
        if (position >= buffer_size) {
            buffer_size += SHELL_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "shell: allocation error!\n");
                exit(EXIT_FAILURE);
            }
        } 
    }
}

#define SHELL_BUFFER_SIZE_TOKEN 64
#define SHELL_DELIMETR_TOKEN " \t\r\n\a"

char** shell_split_line(char* line) {
    int buffer_size = SHELL_BUFFER_SIZE_TOKEN;
    int position = 0;
    char** tokens = malloc(buffer_size * sizeof(char*));
    char* token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_DELIMETR_TOKEN);

    while (token != NULL) {
        tokens[position] = token;
        ++position;

        if (position >= buffer_size) {
            buffer_size += SHELL_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "shell: allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SHELL_DELIMETR_TOKEN);
    }
    tokens[position] = NULL;
    return tokens;   
}

void shell_loop(void) {
    char* line;
    char** args;
    int status;

    do {
        printf("~> ");
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argс, char** argv) {
    shell_loop();

    return EXIT_SUCCESS;
}