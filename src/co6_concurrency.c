
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#define NUM_THREADS 4
#define ITERATIONS 100000

static long long shared_counter = 0;
static pthread_mutex_t counter_mutex;
static atomic_int worker_error = 0;

/* Experiment 1: Intentionally demonstrate an unsafe data race */
static void *unsafe_increment(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        shared_counter++;  /* Intentionally unprotected */
    }

    return NULL;
}

/* Experiment 2: Protect every update with a mutex */
static void *safe_increment(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        int rc = pthread_mutex_lock(&counter_mutex);

        if (rc != 0) {
            atomic_store(&worker_error, 1);
            return NULL;
        }

        shared_counter++;

        rc = pthread_mutex_unlock(&counter_mutex);

        if (rc != 0) {
            atomic_store(&worker_error, 1);
            return NULL;
        }
    }

    return NULL;
}

/* Create and join the requested worker threads */
static int run_threads(void *(*worker)(void *)) {
    pthread_t threads[NUM_THREADS];
    int created = 0;
    int failed = 0;

    atomic_store(&worker_error, 0);

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&threads[i], NULL, worker, NULL);

        if (rc != 0) {
            fprintf(stderr, "Could not create thread %d: %d\n",
                    i + 1, rc);
            failed = 1;
            break;
        }

        created++;
    }

    for (int i = 0; i < created; i++) {
        int rc = pthread_join(threads[i], NULL);

        if (rc != 0) {
            fprintf(stderr, "Could not join thread %d: %d\n",
                    i + 1, rc);
            failed = 1;
        }
    }

    if (atomic_load(&worker_error)) {
        fprintf(stderr, "A worker encountered a synchronization error.\n");
        failed = 1;
    }

    return failed ? -1 : 0;
}

int main(void) {
    const long long expected =
        (long long)NUM_THREADS * ITERATIONS;

    printf("========================================\n");
    printf(" Thread Synchronization Demonstration\n");
    printf("========================================\n");
    printf("Threads                  : %d\n", NUM_THREADS);
    printf("Increments per thread    : %d\n", ITERATIONS);
    printf("Expected counter         : %lld\n", expected);

    /* Experiment 1: Unsafe counter */
    printf("\n[EXPERIMENT 1] Without Mutex\n");
    printf("----------------------------------------");

    shared_counter = 0;

    if (run_threads(unsafe_increment) != 0) {
        fprintf(stderr, "Unsafe experiment could not complete.\n");
        return EXIT_FAILURE;
    }

    printf("\nObserved counter         : %lld\n", shared_counter);
    printf("Expected counter         : %lld\n", expected);
    printf("Result                   : Data race demonstrated\n");
    printf("Note: The observed value is nondeterministic.\n");

    /* Initialize mutex before the safe experiment */
    int rc = pthread_mutex_init(&counter_mutex, NULL);

    if (rc != 0) {
        fprintf(stderr, "Mutex initialization failed: %d\n", rc);
        return EXIT_FAILURE;
    }

    /* Experiment 2: Safe counter */
    printf("\n[EXPERIMENT 2] With Mutex\n");
    printf("----------------------------------------");

    shared_counter = 0;

    if (run_threads(safe_increment) != 0) {
        fprintf(stderr, "Safe experiment could not complete.\n");
        pthread_mutex_destroy(&counter_mutex);
        return EXIT_FAILURE;
    }

    printf("\nObserved counter         : %lld\n", shared_counter);
    printf("Expected counter         : %lld\n", expected);

    int verification_passed = (shared_counter == expected);

    if (verification_passed) {
        printf("Verification             : PASS\n");
        printf("Mutex prevented lost updates.\n");
    } else {
        printf("Verification             : FAIL\n");
    }

    rc = pthread_mutex_destroy(&counter_mutex);

    if (rc != 0) {
        fprintf(stderr, "Mutex destruction failed: %d\n", rc);
        return EXIT_FAILURE;
    }

    printf("\n========================================\n");
    printf(" Concurrency Test Complete\n");
    printf("========================================\n");

    return verification_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
