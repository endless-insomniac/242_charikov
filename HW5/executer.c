#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void run_command_and_count_bytes(const char *cmd) {
    int pfd[2];

    if (pipe(pfd) == -1) {
        die("pipe");
    }

    pid_t cmd_pid = fork();

    if (cmd_pid == -1) {
        die("fork command");
    }

    if (cmd_pid == 0) {
        if (close(pfd[0]) == -1) {
            perror("close");
            _exit(EXIT_FAILURE);
        }

        if (dup2(pfd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        if (close(pfd[1]) == -1) {
            perror("close");
            _exit(EXIT_FAILURE);
        }

        execl("/bin/sh", "sh", "-c", cmd, (char *) NULL);

        perror("execl");
        _exit(EXIT_FAILURE);
    }

    pid_t wc_pid = fork();

    if (wc_pid == -1) {
        die("fork wc");
    }

    if (wc_pid == 0) {
        if (close(pfd[1]) == -1) {
            perror("close");
            _exit(EXIT_FAILURE);
        }

        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        if (close(pfd[0]) == -1) {
            perror("close");
            _exit(EXIT_FAILURE);
        }

        execlp("wc", "wc", "-c", (char *) NULL);

        perror("execlp");
        _exit(EXIT_FAILURE);
    }

    if (close(pfd[0]) == -1) {
        die("close");
    }

    if (close(pfd[1]) == -1) {
        die("close");
    }

    int status;

    if (waitpid(cmd_pid, &status, 0) == -1) {
        die("waitpid command");
    }

    if (waitpid(wc_pid, &status, 0) == -1) {
        die("waitpid wc");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"command0 args\" \"command1 args\" ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        run_command_and_count_bytes(argv[i]);
    }

    return EXIT_SUCCESS;
}