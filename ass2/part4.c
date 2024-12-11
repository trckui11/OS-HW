#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links");
    fprintf(stderr, "  -p: Copy file permissions");
}

int main(int argc, char *argv[]) {
    int copy_symlinks = 0;
    int copy_permissions = 0;

    // make sure there are at least 2 args, for flag check
    if (argc <= 2) {
        print_usage(argv[0]);
        return 1;
    }
    // Set flags
    if (strcmp(argv[1], "-l") == 0) {
        copy_symlinks = 1;
    }
    if (strcmp(argv[1], "-p") == 0) {
        copy_permissions = 1;
    }
    if (strcmp(argv[2], "-l") == 0) {
        copy_symlinks = 1;
    }
    if (strcmp(argv[2], "-p") == 0) {
        copy_permissions = 1;
    }

    int flags = copy_symlinks + copy_permissions;
    if (flags + 3 != argc) {
        print_usage(argv[0]);
        return 1;
    }

    const char *src_dir = argv[flags + 1];
    const char *dest_dir = argv[flags + 2];

    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}