
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define FILE_NAME "database_record.txt"
#define MESSAGE "Readers-Writers Database Simulator\n"

int main(void) {
    printf("=== CO-5: File Systems and File I/O ===\n\n");

    // Create/open file using Linux system calls
    int fd = open(FILE_NAME, O_CREAT | O_RDWR | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("File created: %s\n", FILE_NAME);
    printf("File descriptor: %d\n", fd);

    // Unbuffered file I/O using write()
    ssize_t bytes_written = write(fd, MESSAGE, strlen(MESSAGE));

    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Bytes written: %zd\n", bytes_written);

    // Retrieve file metadata
    struct stat file_info;

    if (fstat(fd, &file_info) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("\nFile metadata:\n");
    printf("Inode number: %lu\n",
           (unsigned long)file_info.st_ino);
    printf("File size: %ld bytes\n",
           (long)file_info.st_size);
    printf("Permissions: %o\n",
           (unsigned int)(file_info.st_mode & 0777));

    // Read file using lseek() and read()
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return EXIT_FAILURE;
    }

    char buffer[128] = {0};
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("\nContent read using read():\n");
    printf("%s", buffer);

    // Memory-mapped file demonstration
    if (file_info.st_size > 0) {
        void *mapped = mmap(NULL, file_info.st_size,
                            PROT_READ, MAP_PRIVATE, fd, 0);

        if (mapped == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return EXIT_FAILURE;
        }

        printf("\nContent accessed through mmap():\n");
        printf("%.*s", (int)file_info.st_size, (char *)mapped);

        if (munmap(mapped, file_info.st_size) == -1) {
            perror("munmap");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    printf("\nFile descriptor closed successfully.\n");
    printf("CO-5 demonstration completed.\n");

    return EXIT_SUCCESS;
}

