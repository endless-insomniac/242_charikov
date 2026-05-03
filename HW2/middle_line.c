#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <errno.h>

#define STDOUT_FD 1
#define STDERR_FD 2

static void write_str(int fd, const char *s) {
    while (*s) {
        ssize_t written = write(fd, s, 1);
        if (written < 0 && errno != EINTR) {
            return;
        }
        if (written > 0) {
            s++;
        }
    }
}

static int write_all(int fd, const char *buf, size_t size) {
    size_t total = 0;

    while (total < size) {
        ssize_t written = write(fd, buf + total, size - total);

        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        total += (size_t)written;
    }

    return 0;
}

static size_t choose_buffer_size(off_t file_size) {
    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0) {
        page_size = 4096;
    }

    if (file_size > 0 && file_size < page_size) {
        return (size_t)file_size;
    }

    return (size_t)page_size;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        write_str(STDERR_FD, "Usage: ./middle_line <file>\n");
        return 1;
    }

    const char *filename = argv[1];

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        write_str(STDERR_FD, "Error: cannot open file\n");
        return 1;
    }

    struct stat st;

    if (fstat(fd, &st) < 0) {
        write_str(STDERR_FD, "Error: fstat failed\n");
        close(fd);
        return 1;
    }

    off_t middle = st.st_size / 2;

    if (lseek(fd, middle, SEEK_SET) < 0) {
        write_str(STDERR_FD, "Error: lseek failed\n");
        close(fd);
        return 1;
    }

    size_t buffer_size = choose_buffer_size(st.st_size);
    char *buffer = malloc(buffer_size);

    if (buffer == NULL) {
        write_str(STDERR_FD, "Error: malloc failed\n");
        close(fd);
        return 1;
    }

    int done = 0;

    while (!done) {
        ssize_t bytes_read = read(fd, buffer, buffer_size);

        if (bytes_read < 0) {
            if (errno == EINTR) {
                continue;
            }

            write_str(STDERR_FD, "Error: read failed\n");
            free(buffer);
            close(fd);
            return 1;
        }

        if (bytes_read == 0) {
            break;
        }

        size_t bytes_to_write = (size_t)bytes_read;

        for (size_t i = 0; i < (size_t)bytes_read; i++) {
            if (buffer[i] == '\n') {
                bytes_to_write = i + 1;
                done = 1;
                break;
            }
        }

        if (write_all(STDOUT_FD, buffer, bytes_to_write) < 0) {
            write_str(STDERR_FD, "Error: write failed\n");
            free(buffer);
            close(fd);
            return 1;
        }
    }

    free(buffer);

    if (close(fd) < 0) {
        write_str(STDERR_FD, "Error: close failed\n");
        return 1;
    }

    return 0;
}