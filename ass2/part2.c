#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

int main(int argc, char *argv[]) {
    // Check input
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
        return 1;
    }

    int count = atoi(argv[argc - 1]);
    pid_t pid;
    // Fork all children
    for (int i = 1; i < argc - 1; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        if (pid == 0) { // Child process
            // Check if lockfile exists: if it does - wait & try again. otherwise - create it
            int fd;
            do {
                errno = 0;
                fd = open("lockfile.lock", O_CREAT | O_EXCL, 0666);
                usleep(5);
            } while (errno == EEXIST);
            if (fd == -1) {
                perror("open failed");
                return 1;
            }
            write_message(argv[i], count);
            remove("lockfile.lock");
            return 0;
        }
        // Parent process continues with the loop
    }

    // Wait for all children
    for (int i = 0; i < argc - 2; i++) {
        wait(NULL);
    }

    return 0;
}