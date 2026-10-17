
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void) {
    setbuf(stdout, NULL);

    printf("=== CO-4: Memory Management ===\n");
    printf("Parent PID: %ld\n", (long)getpid());

    // Dynamic memory allocation
    int *data = malloc(5 * sizeof(int));

    if (data == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++) {
        data[i] = (i + 1) * 10;
    }

    printf("\nDynamically allocated values:\n");

    for (int i = 0; i < 5; i++) {
        printf("data[%d] = %d\n", i, data[i]);
    }

    printf("\nProcess memory information:\n");

    char command[128];
    snprintf(command, sizeof(command),
             "grep -E 'VmSize|VmRSS|VmData' /proc/%ld/status",
             (long)getpid());

    printf("Reading /proc/PID/status:\n");
    system(command);

    // Demonstrate copy-on-write
    printf("\n=== Copy-on-Write Demonstration ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        free(data);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("Child PID: %ld\n", (long)getpid());
        printf("Child sees data[0] = %d\n", data[0]);

        data[0] = 999;

        printf("Child changed data[0] to %d\n", data[0]);
        printf("Child exiting.\n");

        free(data);
        _exit(EXIT_SUCCESS);
    }

    int status;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        free(data);
        return EXIT_FAILURE;
    }

    printf("Parent still sees data[0] = %d\n", data[0]);
    printf("Child's modification did not change parent's copy.\n");

    free(data);

    printf("\nDynamic memory released.\n");
    printf("CO-4 demonstration completed.\n");

    return EXIT_SUCCESS;
}

