#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_PATH_LEN 256

// Function declarations
void create_directory(const char *dir_name, mode_t mode);
void create_write_file(const char *file_name, const char *content);
void read_proc_mem_info_to_file(const char *file_name);
void directory_listing(const char *start_dir);
void remove_directory(const char *dir_name);
void clear_input_buffer(void);

int main() {
    int choice;
    char dir_name[MAX_PATH_LEN];
    char txt_output[MAX_PATH_LEN];
    char filename[MAX_PATH_LEN];
    char dir_to_list[MAX_PATH_LEN];
    char choice_char;
    mode_t mode;

    while (choice != 99) {
        printf("\n\n ----- Menu -----");
        printf("\n 1. Create a directory");
        printf("\n 2. Create and write to a file");
        printf("\n 3. Write contents of /proc/meminfo to a file");
        printf("\n 4. List directory contents");
        printf("\n 5. Remove a directory");
        printf("\n 99: Quit");
        printf("\n Enter your choice: ");
        scanf("%d", &choice);

        printf("\n");
        clear_input_buffer();

        switch (choice) {
            case 1:
                printf("\n Selected: Create a directory with a given name and permissions.");
                printf("\n Enter directory name you want to create: ");
                scanf("%s", dir_name);
                clear_input_buffer();
                printf("\n Enter the mode of the created directory (in octal, e.g., 0755): ");
                scanf("%o", &mode);
                clear_input_buffer();
                create_directory(dir_name, mode);
                printf("\n The directory %s is successfully created with mode %o", dir_name, mode);
                break;
            case 2:
                printf("\n Selected: Create a file with a given name and write text to the file.");
                printf("\n Enter the file name you want to create: ");
                scanf("%s", dir_name);

                printf("\n Enter the contents that you want to write to the file: ");
                scanf(" %[^\n]", txt_output);
                clear_input_buffer();
                create_write_file(dir_name, txt_output);
                printf("\n File %s is created and written successfully", dir_name);
                break;
            case 3:
                printf("\n Selected: Extract and save key information from /proc/meminfo to a file.\n");
                printf("\n Enter the file name you want to store memory information to: ");
                scanf("%s", filename);
                clear_input_buffer();
                read_proc_mem_info_to_file(filename);
                printf("Proc info written to %s successfully.\n", filename);
                break;
            case 4:
                printf("\n Selected: List the contents of a directory.\n");
                printf("\n Enter the directory name you want to list the contents for (if you want to list the contents of the current directory, then pass '.'): ");
                scanf("%s", dir_to_list);
                clear_input_buffer();
                directory_listing(dir_to_list);
                printf("\n Directory contents listed for %s successfully.\n", dir_to_list);
                break;
            case 5:
                printf("\n Selected: Remove a directory.");
                printf("\n Enter directory name you want to remove: ");
                scanf("%s", dir_name);
                clear_input_buffer();
                printf("\n Warning: You are about to delete the directory %s and all its contents recursively. Proceed? (y/n) ", dir_name);
                scanf(" %c", &choice_char);
                clear_input_buffer();
                if (choice_char == 'y') {
                    remove_directory(dir_name);
                    printf("\n Deleted.");
                } else {
                    printf("\n Aborted.");
                }
                break;
            case 99:
                printf("\n Exiting...");
                exit(0);
                break;
            default:
                printf("\n Invalid choice");
        }
    }
}

void create_directory(const char *dir_name, mode_t mode) {
    if (mkdir(dir_name, mode) == -1) {
        perror("Error creating directory");
    }
}

void create_write_file(const char *file_name, const char *content) {
    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        exit(1);
    }

    if (write(fd, content, strlen(content)) == -1) {
        perror("Error writing to file");
        close(fd);
        exit(1);
    }

    close(fd);
}

void read_proc_mem_info_to_file(const char *file_name) {
    int source_fd = open("/proc/meminfo", O_RDONLY);
    if (source_fd == -1) {
        perror("Failed to open /proc/meminfo");
        return;
    }

    int dest_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Failed to open destination file");
        close(source_fd);
        return;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("Failed to write to destination file");
            break;
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from /proc/meminfo");
    }

    close(source_fd);
    close(dest_fd);
}

void directory_listing(const char *start_dir) {
    DIR *dir;
    struct dirent *entry;
    char path[1024];
    struct stat statbuf;

    dir = opendir(start_dir);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(path, sizeof(path), "%s/%s", start_dir, entry->d_name);
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            printf("%s/ (dir)\n", path);
        } else {
            printf("%s\n", path);
        }
    }

    closedir(dir);
}

void remove_directory(const char *dir_name) {
    if (rmdir(dir_name) == -1) {
        perror("Error removing directory");
    }
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}