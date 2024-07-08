#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5
#define MAX_NUMBER 100

// Union for semaphore operations
union semun {
    int val;                    // Value for SETVAL
    struct semid_ds* buf;       // Buffer for IPC_STAT, IPC_SET
    unsigned short* array;      // Array for GETALL, SETALL
};

// Function for P (wait) operation on semaphore
void P(int semid, int sem_num) {
    struct sembuf sb = { sem_num, -4, 0 }; // Decrease semaphore by 4
    semop(semid, &sb, 1); // Perform operation
}

// Function for V (signal) operation on semaphore
void V(int semid, int sem_num) {
    struct sembuf sb = { sem_num, 1, 0 }; // Increase semaphore by 1
    semop(semid, &sb, 1); // Perform operation
}

int main() {
    int semid, i, status;
    pid_t pid;

    // Create semaphore set
    semid = semget(IPC_PRIVATE, NUM_PROCESSES, 0666 );
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // Initialize semaphores with values 4, 3, 2, 1, 0
    union semun arg;
    unsigned short values[NUM_PROCESSES] = { 4, 3, 2, 1, 0 };
    arg.array = values;
    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // Create child processes
    for (i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) { // Child process
            for (int j = i + 1; j <= MAX_NUMBER; j += NUM_PROCESSES) {
                P(semid, i); // Perform P operation on the current semaphore
                printf("%d-", j); // Print the number
                fflush(stdout); // Flush the output buffer

                // Perform V operation on all other semaphores
                for (int k = 0; k < NUM_PROCESSES; k++) {
                    if (k != i) {
                        V(semid, k);
                    }
                }
            }
            exit(0); // Exit child process
        }
    }

    // Wait for all child processes to finish
    for (i = 0; i < NUM_PROCESSES; i++) {
        wait(&status);
    }

    printf("\n"); // Print a new line after all processes are done

    // Remove semaphore set
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }

    return 0;
}

