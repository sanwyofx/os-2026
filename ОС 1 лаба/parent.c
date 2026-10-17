#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFF_SIZE 4096

int main() {
    int pipe1[2];
    int pipe3[2];
    int pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe3) == -1 || pipe(pipe2) == -1) {
        perror("parent: pipe creation error");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("parent: fork1 error");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        if (dup2(pipe1[0], STDIN_FILENO) == -1 || dup2(pipe3[1], STDOUT_FILENO) == -1) {
            perror("child1: dup2 error");
            exit(EXIT_FAILURE);
        }

        close(pipe1[0]); close(pipe1[1]);
        close(pipe3[0]); close(pipe3[1]);
        close(pipe2[0]); close(pipe2[1]);

        execl("./child1", "child1", NULL);
        perror("child1: execl error");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("parent: fork2 error");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        if (dup2(pipe3[0], STDIN_FILENO) == -1 || dup2(pipe2[1], STDOUT_FILENO) == -1) {
            perror("child2: dup2 error");
            exit(EXIT_FAILURE);
        }

        close(pipe1[0]); close(pipe1[1]);
        close(pipe3[0]); close(pipe3[1]);
        close(pipe2[0]); close(pipe2[1]);

        execl("./child2", "child2", NULL);
        perror("child2: execl error");
        exit(EXIT_FAILURE);
    }

    close(pipe1[0]);
    close(pipe3[0]);
    close(pipe3[1]);
    close(pipe2[1]);

    char buffer[BUFF_SIZE];
    char out_buffer[BUFF_SIZE];
    ssize_t bytes_read;

    const char *test = "Введите строки:\n";
    write(STDOUT_FILENO, test, strlen(test));

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0) {
        if (write(pipe1[1], buffer, (size_t)bytes_read) < 0) {
            perror("parent: write to pipe1 error");
            break;
        }

        size_t total_received = 0;
        while (total_received < (size_t)bytes_read) {
            ssize_t res = read(pipe2[0], out_buffer, (size_t)bytes_read - total_received);
            if (res <= 0) break;
            write(STDOUT_FILENO, out_buffer, (size_t)res);
            total_received += (size_t)res;
        }
    }

    close(pipe1[1]);
    close(pipe2[0]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return EXIT_SUCCESS;
}
