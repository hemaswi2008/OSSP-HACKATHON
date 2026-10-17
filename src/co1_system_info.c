
#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

int main(void) {
    struct utsname system_info;

    printf("=== CO-1: OS as a Service Layer ===\n\n");

    // Process information from the Linux kernel
    printf("Process ID (PID): %ld\n", (long)getpid());
    printf("Parent Process ID (PPID): %ld\n", (long)getppid());
    printf("User ID (UID): %ld\n", (long)getuid());

    // Operating system and kernel information
    if (uname(&system_info) == 0) {
        printf("\nOperating System: %s\n", system_info.sysname);
        printf("Kernel Release: %s\n", system_info.release);
        printf("Machine Architecture: %s\n", system_info.machine);
    } else {
        perror("uname");
        return 1;
    }

    // System configuration
    long processors = sysconf(_SC_NPROCESSORS_ONLN);

    if (processors != -1) {
        printf("Online Processors: %ld\n", processors);
    }

    printf("\nCO-1 demonstration completed.\n");

    return 0;
}
