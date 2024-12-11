#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void write_message(int output_fd, char *buffer, int count) {
    for (int i = 0; i < count; i++) {
        if (write(output_fd, buffer, strlen(buffer)) == -1) {
            perror("write failed");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    // Check input
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>\n", argv[0]);
        return 1;
    }
    char *parent_message = argv[1];
    char *child1_message = argv[2];
    char *child2_message = argv[3];
    int count = atoi(argv[4]);

    // Create output file
    int fd = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid1 == 0) { // Child 1 process
        write_message(fd, child1_message, count);
        return 0;
    }
    // Parent process

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid2 == 0) { // Child 2 process
        sleep(1); // Ensure child 1 writes first (Naive synchronisation)
        write_message(fd, child2_message, count);
        return 0;
    }
    // Parent process
    
    // Wait for childern
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    write_message(fd, parent_message, count);

    // Close file
    if (close(fd) == -1) {
        perror("close failed");
        return 1;
    }

    return 0;
}