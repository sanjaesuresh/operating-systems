#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024

void copy_file(const char *input_path, const char *output_path) {
    int input_fd, output_fd;
    ssize_t read_content, write_content;
    char buffer[BUFFER_SIZE];

    input_fd = open(input_path, O_RDONLY);
    if (input_fd == -1) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("Failed to open output file");
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    while ((read_content = read(input_fd, buffer, sizeof(buffer))) > 0) {
        write_content = write(output_fd, buffer, read_content);
        if (write_content != read_content) {
            perror("Failed to write to output file");
            close(input_fd);
            close(output_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (read_content == -1) {
        perror("Error reading input file");
        close(input_fd);
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    close(input_fd);
    close(output_fd);
    printf("The contents of file %s have been successfully copied into %s file.\n", input_path, output_path);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Insufficient parameters passed.");
        return EXIT_FAILURE;
    }

    copy_file(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
