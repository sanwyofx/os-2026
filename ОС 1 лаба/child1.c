#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define BUFF_SIZE 4096

int main() {
    char buffer[BUFF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        ssize_t total_written = 0;
        while (total_written < bytes_read) {
            ssize_t bytes_written = write(STDOUT_FILENO, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                perror("child1: write error");
                return EXIT_FAILURE;
            }
            total_written += bytes_written;
        }
    }

    if (bytes_read < 0) {
        perror("child1: read error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
