#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <errno.h>
#include <limits.h>

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

static long parse_positive_long(const char *s, int allow_zero) {
    char *end = NULL;
    errno = 0;

    long value = strtol(s, &end, 10);

    if (errno != 0 || end == s || *end != '\0') {
        return -1;
    }

    if (allow_zero) {
        if (value < 0) {
            return -1;
        }
    } else {
        if (value <= 0) {
            return -1;
        }
    }

    return value;
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
    if (argc != 4) {
        write_str(STDERR_FD, "Usage: ./print_lines <file> <start_line> <number_of_lines>\n");
        return 1;
    }

    const char *filename = argv[1];

    long start_line = parse_positive_long(argv[2], 0);
    long number_of_lines = parse_positive_long(argv[3], 1);

    if (start_line < 0 || number_of_lines < 0) {
        write_str(STDERR_FD, "Error: start_line must be positive, number_of_lines must be non-negative\n");
        return 1;
    }

    if (number_of_lines == 0) {
        return 0;
    }

    long after_last_line = start_line + number_of_lines;

    if (after_last_line < start_line) {
        write_str(STDERR_FD, "Error: integer overflow\n");
        return 1;
    }

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

    size_t buffer_size = choose_buffer_size(st.st_size);
    char *buffer = malloc(buffer_size);

    if (buffer == NULL) {
        write_str(STDERR_FD, "Error: malloc failed\n");
        close(fd);
        return 1;
    }

    long current_line = 1;
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

        size_t output_start = 0;
        size_t output_end = 0;
        int has_output = 0;

        for (size_t i = 0; i < (size_t)bytes_read; i++) {
            if (current_line >= start_line && current_line < after_last_line) {
                if (!has_output) {
                    output_start = i;
                    has_output = 1;
                }

                output_end = i + 1;
            }

            if (buffer[i] == '\n') {
                current_line++;

                if (current_line >= after_last_line && has_output) {
                    done = 1;
                    break;
                }
            }
        }

        if (has_output) {
            if (write_all(STDOUT_FD, buffer + output_start, output_end - output_start) < 0) {
                write_str(STDERR_FD, "Error: write failed\n");
                free(buffer);
                close(fd);
                return 1;
            }
        }
    }

    free(buffer);

    if (close(fd) < 0) {
        write_str(STDERR_FD, "Error: close failed\n");
        return 1;
    }

    return 0;
}