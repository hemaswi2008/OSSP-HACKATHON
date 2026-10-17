
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    printf("=== CO-2: Process Management ===\n");
    printf("Parent PID: %ld\n", (long)getpid());
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process
        printf("\nChild process created.\n");
        printf("Child PID: %ld\n", (long)getpid());
        printf("Parent PID: %ld\n", (long)getppid());

        printf("Child executing the 'ls' command...\n");
        execlp("ls", "ls", "-l", "src", (char *)NULL);

        // Reached only if exec fails
        perror("execlp");
        _exit(EXIT_FAILURE);
    } else {
        // Parent process
        printf("\nParent waiting for child PID: %ld\n",
               (long)pid);

        int status;
        pid_t result = waitpid(pid, &status, 0);

        if (result == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }

        if (WIFEXITED(status)) {
            printf("\nChild exited with status: %d\n",
                   WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("\nChild terminated by signal: %d\n",
                   WTERMSIG(status));
        }

        printf("Parent process completed.\n");
    }

    return EXIT_SUCCESS;
}
