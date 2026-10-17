#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define BUF_SIZE 4096
#define REPLACE_CHAR '_'

int main() {
    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            unsigned char c = (unsigned char)buffer[i];
            if (isspace(c) && c != '\n') {
                buffer[i] = REPLACE_CHAR;
            }
        }

        ssize_t total_written = 0;
        while (total_written < bytes_read) {
            ssize_t bytes_written = write(STDOUT_FILENO, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                perror("child2: write error");
                return EXIT_FAILURE;
            }
            total_written += bytes_written;
        }
    }

    if (bytes_read < 0) {
        perror("child2: read error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
