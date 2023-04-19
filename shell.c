#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10
extern char **environ;

char *find_command_path(char *command) {
    char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }
    char *dir = strtok(path, ":");
    while (dir != NULL) {
        char *full_path = malloc(strlen(dir) + strlen(command) + 2);
        sprintf(full_path, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            return full_path;
        }
        free(full_path);
        dir = strtok(NULL, ":");
    }
    return NULL;
}

int main() {
    char buffer[BUFFER_SIZE];
    char *args[BUFFER_SIZE/2 + 1];
    int status;
    pid_t pid;
    char *user = getenv("USER");

    while (1) {
        printf("^_^ %s$ ", user);
        fflush(stdout);
        if (!fgets(buffer, BUFFER_SIZE, stdin)) {
            break;
        }
        // Remove trailing newline character from input
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        // Tokenize the input into arguments
        char *token;
        int i = 0;
        token = strtok(buffer, " ");
        while (token != NULL) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;
        // Check for built-in commands
        if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) == -1) {
                perror("chdir");
            }
            continue;
        } else if (strcmp(args[0], "exit") == 0) {
            printf("^_^ till next time %s... \n", user);
            break; // exit the while loop
        } else if (strcmp(args[0], "env") == 0) {
            char **env = environ;
            while (*env != NULL) {
                printf("%s\n", *env);
                env++;
            }
            continue;
        } else if (strcmp(args[0], "touch") == 0) {
            if (args[1] == NULL) {
                printf("Usage: touch [filename]\n");
                continue;
            }
            int fd = open(args[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                perror("open");
                continue;
            }
            close(fd);
            continue;
        }
        // Check if command exists in PATH
        char *command_path = find_command_path(args[0]);
        if (command_path == NULL) {
            printf("Command not found: %s\n", args[0]);
            continue;
        }
        // Fork a child process to execute the command
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            if (execve(command_path, args, NULL) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            if (wait(&status) == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }
        }
    }
    exit(EXIT_SUCCESS);
    return 0;
}
