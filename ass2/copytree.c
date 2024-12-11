#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    // firstly, handle soft links
    struct stat buf;
    if (copy_symlinks) {
        if (lstat(src, &buf) == -1) {
            perror("lstat failed");
            return;
        }
        if (S_ISLNK(buf.st_mode)) {
            // get target of link
            char link_path[1024];
            ssize_t len = readlink(src, link_path, 1024);
            if (len == -1) {
                perror("readlink failed");
                return;
            }
            link_path[len] = '\0';

            // create the copy of the soft link
            if (symlink(link_path, dest) == -1) {
                perror("symlink failed");
                return;
            }

            return;
        }
    }

    int fd = open(src, O_RDONLY, 0666);
    if (fd == -1) {
        perror("open failed");
        return;
    }

    // read file content into buffer
    off_t file_length = lseek(fd, 0, SEEK_END);
    if (file_length == -1) {
        perror("lseek failed");
        close(fd);
        return;
    }
    char *file_content = (char *)malloc(file_length);
    if (!file_content) {
        perror("malloc failed");
        close(fd);
        return;
    }
    
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek failed");
        close(fd);
        return;
    }
    if (read(fd, file_content, file_length) != file_length) {
        perror("read failed");
        free(file_content);
        close(fd);
        return;
    }

    // original file stream is not needed anymore
    if (close(fd) == -1) {
        perror("close failed");
        return;
    }

    // handle permissions & create copy file
    int copy_fd;
    if (copy_permissions) {
        if (stat(src, &buf) == -1) {
            perror("stat failed");
            free(file_content);
            return;
        }
        copy_fd = open(dest, O_WRONLY | O_CREAT | O_EXCL, buf.st_mode);
    } else {
        copy_fd = open(dest, O_WRONLY | O_CREAT | O_EXCL, 0666);
    }

    if (copy_fd == -1) {
        perror("open failed");
        free(file_content);
        return;
    }

    // write content into copy file
    if (write(copy_fd, file_content, file_length) != file_length) {
        perror("write failed");
        free(file_content);
        close(copy_fd);
        return;
    }

    free(file_content);
    if (close(copy_fd) == -1) {
        perror("close failed");
    }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *pDir = opendir(src);
	if (pDir == NULL) {
        perror("opendir failed");
        return;
    }
	struct dirent *pDirent; 

    // handle soft links
    struct stat buf;
    if (copy_symlinks) {
        if (lstat(src, &buf) == -1) {
            perror("lstat failed");
            return;
        }

        if (S_ISLNK(buf.st_mode)) {
            // get target of link
            char link_path[1024];
            ssize_t len = readlink(src, link_path, 1024);
            if (len == -1) {
                perror("readlink failed");
                return;
            }
            link_path[len] = '\0';

            // create the copy of the soft link
            if (symlink(link_path, dest) == -1) {
                perror("symlink failed");
                return;
            }

            if (closedir(pDir) == -1) {
                perror("closedir failed");
                return;
            }
            return;
        }
    }

    // handle permissions
    mode_t mode = 0777;
    if (copy_permissions) {
        if (stat(src, &buf) == -1) {
            perror("stat failed");
            closedir(pDir);
            return;
        }
        mode = buf.st_mode;
    }
    // create empty dir
    if (mkdir(dest, mode) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        closedir(pDir);
        return;
    }

	// iterate through directory
	while ((pDirent = readdir(pDir)) != NULL) {
        // make src not const
        char new_src[1024];
        strcpy(new_src, src);
        new_src[strlen(src)] = '\0';

        // make dest not const
        char new_dest[1024];
        strcpy(new_dest, dest);
        new_dest[strlen(dest)] = '\0';

        // ignore . and ..
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }
        
        char *src_path = strcat(strcat(new_src, "/"), pDirent->d_name);
        char *dest_path = strcat(strcat(new_dest, "/"), pDirent->d_name);
        if (pDirent->d_type == DT_DIR) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    } 
	if (closedir(pDir) == -1) {
        perror("closedir failed");
        return;
    }
}