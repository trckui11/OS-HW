#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_NUMBER 100
#define MAX_COMMAND_LENGTH 100

char *history[MAX_COMMAND_NUMBER];
int history_length = 0;

void std_command(char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv) == -1) {
            perror("exec failed");
        }
        exit(1);
    } else {
        // Parent process
        if (waitpid(pid, NULL, 0) != pid) {
            perror("waitpid failed");
            exit(1);
        }
    }
}

void free_history() {
    for (int i = 0; i < history_length; i++) {
        free(history[i]);
    }
}

int main(int argc, char *argv[]) {
    atexit(free_history);
    
    // Update PATH:
    char *newPATH = getenv("PATH");
    if (!newPATH) {
        perror("getenv failed");
    }
    for (int i = 1; i < argc; i++) {
        strcat(newPATH, ":");
        strcat(newPATH, argv[i]);
    }
    setenv("PATH", newPATH, 1);

    while (1) {
        printf("$ ");
        fflush(stdout);

        char input[MAX_COMMAND_LENGTH];
        scanf(" %[^\n]", input);

        char input_copy[MAX_COMMAND_LENGTH];
        strcpy(input_copy, input);
        char *args[MAX_COMMAND_LENGTH + 1]; // each arg must be at least 1 char, and a place for NULL
        // Split input into args
        char *token = strtok(input_copy, " ");
        int num_of_args = 0;
        while (token) {
            args[num_of_args++] = token;
            token = strtok(NULL, " ");
        }
        char *command = args[0];
        // Add command to history
        if (history_length < MAX_COMMAND_NUMBER) {
            history[history_length] = malloc(strlen(input) + 1);
            if (history[history_length] == NULL) {
                perror("malloc failed");
                exit(1);
            }
            strcpy(history[history_length++], input);
        }

        // Check for custom functions:
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "cd") == 0) {
            if (chdir(args[1]) != 0) {
                perror("chdir failed");
            }
        } else if (strcmp(command, "pwd") == 0) {
            char cwd[200];
            if (getcwd(cwd, sizeof(cwd))) {
                printf("%s\n", cwd);
            } else {
                perror("getcwd failed");
            }
        } else if (strcmp(command, "history") == 0) {
            // Print history
            for (int i = 0; i < history_length; i++) {
                printf("%s\n", history[i]);
            }
        } else {
            args[num_of_args] = NULL;
            std_command(args);
        }
    }

    return 0;
}