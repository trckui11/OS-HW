#include "buffered_open.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    // If a file creation is needed, pass on the mode
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
    }

    // Remove O_PREAPPEND flag
    int new_flags = flags;
    new_flags &= ~O_PREAPPEND;

    // Open file regularly
    int fd = open(pathname, new_flags, mode);
    if (fd == -1) {
        return NULL;
    }

    // Create buffer
    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf) {
        close(fd);
        return NULL;
    }
    // Initialize it
    bf->fd = fd;
    bf->read_buffer = (char *)malloc(BUFFER_SIZE);
    bf->write_buffer = (char *)malloc(BUFFER_SIZE);
    if (!(bf->read_buffer && bf->write_buffer)) {
        close(fd);
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return NULL;
    }
    bf->read_buffer_size = BUFFER_SIZE;
    bf->write_buffer_size = BUFFER_SIZE;
    // Set to start of file
    bf->read_buffer_pos = 0;
    bf->write_buffer_pos = 0;
    bf->flags = flags;
    // Check if preappending is needed
    bf->preappend = (flags & O_PREAPPEND) ? 1 : 0;

    return bf;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {
    if (bf->preappend) {
        // No room in the buffer
        if (bf->write_buffer_pos + count > bf->write_buffer_size) {
            if (bf->write_buffer_pos > 0) {
                if (buffered_flush(bf) == -1) {
                    return -1;
                }
                bf->write_buffer_pos = 0;
            }

            // Handle O_PREAPPEND
            int file_length = lseek(bf->fd, 0, SEEK_END);
            if (file_length == -1) {
                return -1;
            }

            char *temp_buffer = (char *)malloc(file_length);
            if (!temp_buffer) {
                return -1;
            }

            // Read existing content
            lseek(bf->fd, 0, SEEK_SET);
            if (read(bf->fd, temp_buffer, file_length) == -1) {
                free(temp_buffer);
                return -1;
            }

            // Write new content at the beginning
            lseek(bf->fd, 0, SEEK_SET);
            ssize_t written = write(bf->fd, buf, count);
            if (written == -1) {
                free(temp_buffer);
                return -1;
            }

            // Append existing content
            if (write(bf->fd, temp_buffer, file_length) == -1) {
                free(temp_buffer);
                return -1;
            }

            free(temp_buffer);
            return written;
        }
        // preappend into write buffer
        if (bf->write_buffer_pos > 0) {
            char *temp_buffer = (char *)malloc(bf->write_buffer_pos);
            strncpy(temp_buffer, bf->write_buffer, bf->write_buffer_pos);
            strncpy(bf->write_buffer, buf, count);
            strncpy(bf->write_buffer + count, temp_buffer, strlen(temp_buffer));
        } else {
            strncpy(bf->write_buffer, buf, count);
        }
        bf->write_buffer_pos += count;
        lseek(bf->fd, count, SEEK_SET);
        return count;
    }
    // If not enough room in the write buffer, flush it
    if (bf->write_buffer_pos + count > bf->write_buffer_size) {
        if (buffered_flush(bf) == -1) {
            return -1;
        }
        bf->write_buffer_pos = 0;
    }
    // Handle writing more than buffer size
    if (count > bf->write_buffer_size) {
        return write(bf->fd, buf, count);
    }
    strncpy(bf->write_buffer + bf->write_buffer_pos, buf, count);
    bf->write_buffer_pos += count;
    return count;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    // Firstly, flush the write buffer
    if (bf->write_buffer_pos > 0) {
        if (buffered_flush(bf) == -1) {
            return -1;
        }
    }
    
    // If not enough info in the read buffer or it's empty, renew it
    if (bf->read_buffer_pos + count > bf->read_buffer_size || bf->read_buffer_pos == 0) {
        // overflow check
        if (count > bf->read_buffer_size) {
            bf->read_buffer_pos = 0;
            return read(bf->fd, buf, count);
        }

        // Read chunk from file
        ssize_t bytes_read = read(bf->fd, bf->read_buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            return -1;
        }

        // reset OS offset
        lseek(bf->fd, count - bytes_read, SEEK_CUR);
        // reset buffer pos
        bf->read_buffer_pos = count;
        strncpy(buf, bf->read_buffer, count);
        return count - (bf->read_buffer_size - bytes_read);
    }
    // Enough info in buffer, read from it
    strncpy(buf, bf->read_buffer + bf->read_buffer_pos, count);
    bf->read_buffer_pos += count;
    // Update OS offset
    lseek(bf->fd, count, SEEK_CUR);
    return count;
}

int buffered_flush(buffered_file_t *bf) {
    int offset = lseek(bf->fd, 0, SEEK_CUR);
    if (bf->write_buffer_pos > 0) {
        if (bf->preappend) {
        // Handle O_PREAPPEND
        int file_length = lseek(bf->fd, 0, SEEK_END);
        if (file_length == -1) {
            return -1;
        }

        char *temp_buffer = (char *)malloc(file_length);
        if (!temp_buffer) {
            return -1;
        }

        // Read existing content
        lseek(bf->fd, 0, SEEK_SET);
        if (read(bf->fd, temp_buffer, file_length) == -1) {
            free(temp_buffer);
            return -1;
        }

        // Write buffer contents at the beginning
        lseek(bf->fd, 0, SEEK_SET);
        if (write(bf->fd, bf->write_buffer, bf->write_buffer_pos) == -1) {
            free(temp_buffer);
            return -1;
        }

        // Append existing content
        if (write(bf->fd, temp_buffer, file_length) == -1) {
            free(temp_buffer);
            return -1;
        }

        if (lseek(bf->fd, bf->write_buffer_pos, SEEK_SET) == -1) {
            free(temp_buffer);
            return -1;
        }

        free(temp_buffer);
        bf->write_buffer_pos = 0;
        return 0;
        }
        if (write(bf->fd, bf->write_buffer, bf->write_buffer_pos) == -1) {
            return -1;
        }
        bf->write_buffer_pos = 0;
    }

    // Restore original offset
    lseek(bf->fd, offset, SEEK_SET);
    
    return 0;
}

int buffered_close(buffered_file_t *bf) {
    if (buffered_flush(bf) == -1) {
        return -1;
    }
    free(bf->read_buffer);
    free(bf->write_buffer);
    if (close(bf->fd) == -1) {
        return -1;
    }
    free(bf);
    return 0;
}