
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define OPERATIONS  3
#define INITIAL_VALUE 100
#define WRITE_INCREMENT 10

typedef struct {
    int id;
    int failed;
} ThreadArgs;

/* Shared database */
static int database_value = INITIAL_VALUE;
static pthread_rwlock_t db_lock;

/* Portable delay helper */
static void sleep_ms(long milliseconds) {
    struct timespec delay;

    delay.tv_sec = milliseconds / 1000;
    delay.tv_nsec = (milliseconds % 1000) * 1000000L;

    while (nanosleep(&delay, &delay) == -1) {
        /* Continue sleeping for the remaining time. */
    }
}

static void *reader(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    for (int i = 0; i < OPERATIONS; i++) {
        int rc = pthread_rwlock_rdlock(&db_lock);

        if (rc != 0) {
            fprintf(stderr, "Reader %d: lock failed (%d)\n",
                    args->id, rc);
            args->failed = 1;
            return NULL;
        }

        int snapshot = database_value;

        printf("[READ ] Reader %d | Operation %d | Value = %d\n",
               args->id, i + 1, snapshot);

        rc = pthread_rwlock_unlock(&db_lock);

        if (rc != 0) {
            fprintf(stderr, "Reader %d: unlock failed (%d)\n",
                    args->id, rc);
            args->failed = 1;
            return NULL;
        }

        sleep_ms(100);
    }

    return NULL;
}

static void *writer(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    for (int i = 0; i < OPERATIONS; i++) {
        int rc = pthread_rwlock_wrlock(&db_lock);

        if (rc != 0) {
            fprintf(stderr, "Writer %d: lock failed (%d)\n",
                    args->id, rc);
            args->failed = 1;
            return NULL;
        }

        database_value += WRITE_INCREMENT;

        printf("[WRITE] Writer %d | Operation %d | New value = %d\n",
               args->id, i + 1, database_value);

        rc = pthread_rwlock_unlock(&db_lock);

        if (rc != 0) {
            fprintf(stderr, "Writer %d: unlock failed (%d)\n",
                    args->id, rc);
            args->failed = 1;
            return NULL;
        }

        sleep_ms(150);
    }

    return NULL;
}

int main(void) {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    ThreadArgs reader_args[NUM_READERS];
    ThreadArgs writer_args[NUM_WRITERS];

    int readers_created = 0;
    int writers_created = 0;
    int thread_creation_failed = 0;
    int thread_execution_failed = 0;

    int rc = pthread_rwlock_init(&db_lock, NULL);

    if (rc != 0) {
        fprintf(stderr, "Failed to initialize read-write lock: %d\n",
                rc);
        return EXIT_FAILURE;
    }

    printf("========================================\n");
    printf(" Readers-Writers Database Simulator\n");
    printf("========================================\n");
    printf("Initial value : %d\n", INITIAL_VALUE);
    printf("Readers       : %d\n", NUM_READERS);
    printf("Writers       : %d\n", NUM_WRITERS);
    printf("Operations per thread: %d\n\n", OPERATIONS);

    /* Create readers */
    for (int i = 0; i < NUM_READERS; i++) {
        reader_args[i].id = i + 1;
        reader_args[i].failed = 0;

        rc = pthread_create(&readers[i], NULL, reader,
                            &reader_args[i]);

        if (rc != 0) {
            fprintf(stderr, "Could not create reader %d: %d\n",
                    i + 1, rc);
            thread_creation_failed = 1;
            break;
        }

        readers_created++;
    }

    /* Create writers if reader creation did not fail */
    if (!thread_creation_failed) {
        for (int i = 0; i < NUM_WRITERS; i++) {
            writer_args[i].id = i + 1;
            writer_args[i].failed = 0;

            rc = pthread_create(&writers[i], NULL, writer,
                                &writer_args[i]);

            if (rc != 0) {
                fprintf(stderr, "Could not create writer %d: %d\n",
                        i + 1, rc);
                thread_creation_failed = 1;
                break;
            }

            writers_created++;
        }
    }

    /* Join every thread that was successfully created */
    for (int i = 0; i < readers_created; i++) {
        rc = pthread_join(readers[i], NULL);

        if (rc != 0) {
            fprintf(stderr, "Could not join reader %d: %d\n",
                    i + 1, rc);
            thread_execution_failed = 1;
        } else if (reader_args[i].failed) {
            thread_execution_failed = 1;
        }
    }

    for (int i = 0; i < writers_created; i++) {
        rc = pthread_join(writers[i], NULL);

        if (rc != 0) {
            fprintf(stderr, "Could not join writer %d: %d\n",
                    i + 1, rc);
            thread_execution_failed = 1;
        } else if (writer_args[i].failed) {
            thread_execution_failed = 1;
        }
    }

    int expected_value =
        INITIAL_VALUE + writers_created * OPERATIONS * WRITE_INCREMENT;

    printf("\n========================================\n");
    printf(" Execution Summary\n");
    printf("========================================\n");
    printf("Readers created : %d/%d\n",
           readers_created, NUM_READERS);
    printf("Writers created : %d/%d\n",
           writers_created, NUM_WRITERS);
    printf("Final value     : %d\n", database_value);
    printf("Expected value  : %d\n", expected_value);

    int verification_passed =
        !thread_creation_failed &&
        !thread_execution_failed &&
        readers_created == NUM_READERS &&
        writers_created == NUM_WRITERS &&
        database_value == INITIAL_VALUE +
                          NUM_WRITERS * OPERATIONS * WRITE_INCREMENT;

    if (verification_passed) {
        printf("Verification    : PASS\n");
        printf("All threads completed successfully.\n");
    } else {
        printf("Verification    : FAIL\n");
        printf("Check the errors reported above.\n");
    }

    rc = pthread_rwlock_destroy(&db_lock);

    if (rc != 0) {
        fprintf(stderr, "Failed to destroy read-write lock: %d\n", rc);
        return EXIT_FAILURE;
    }

    return verification_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
