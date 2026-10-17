
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

volatile sig_atomic_t signal_received = 0;

// Signal handler for SIGUSR1
void handle_signal(int signum) {
    if (signum == SIGUSR1) {
        signal_received = 1;
    }
}

int main(void) {
    int pipe_fd[2];
    pid_t pid;
    char buffer[100];

    printf("=== CO-3: Inter-Process Communication ===\n");
    fflush(stdout);

    // Register signal handler
    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // Create anonymous pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process: receive data
        close(pipe_fd[1]);

        ssize_t n = read(pipe_fd[0], buffer,
                         sizeof(buffer) - 1);

        if (n < 0) {
            perror("read");
            close(pipe_fd[0]);
            _exit(EXIT_FAILURE);
        }

        buffer[n] = '\0';

        printf("Child PID: %ld\n", (long)getpid());
        printf("Message received: %s\n", buffer);
        fflush(stdout);

        close(pipe_fd[0]);

        // Notify parent using a signal
        if (kill(getppid(), SIGUSR1) == -1) {
            perror("kill");
            _exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);
    } else {
        // Parent process: send data
        close(pipe_fd[0]);

        const char *message =
            "Hello from parent through an anonymous pipe!";

        printf("Parent PID: %ld\n", (long)getpid());

        if (write(pipe_fd[1], message,
                  strlen(message)) == -1) {
            perror("write");
            close(pipe_fd[1]);
            waitpid(pid, NULL, 0);
            return EXIT_FAILURE;
        }

        printf("Parent sent a message through the pipe.\n");
        close(pipe_fd[1]);

int status;
pid_t result;

do {
    result = waitpid(pid, &status, 0);
} while (result == -1 && errno == EINTR);

if (result == -1) {
    perror("waitpid");
    return EXIT_FAILURE;
}

if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    printf("Child completed successfully.\n");
} else {
    printf("Child did not exit successfully.\n");
}

if (signal_received) {
    printf("Parent received SIGUSR1 from child.\n");
} else {
    printf("SIGUSR1 was not received.\n");
}
    }

    return EXIT_SUCCESS;
}

