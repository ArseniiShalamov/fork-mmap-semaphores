#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

// I WANTED TO WRITE YOU A COMMENT IN RUSSIAN FOR YOUR AMUSMENT, AS IT IS NOT FROM RIGHT TO LEFT ;)

union semun {
    int val;
};

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    int number_of_processes = atoi(argv[argc - 1]), i, shared_memory, semafor;

    shared_memory = shmget(IPC_PRIVATE, sizeof(long long), IPC_CREAT | 0666); //create shared memory
    long long *sum = (long long*)shmat(shared_memory, NULL, 0); // attach shared memory to process
    if (sum == (void*)-1) return 1; // check if shmat was successful
    *sum = 0;

    semafor = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    // semctl(semafor, 0, SETVAL, 1); // initialize semaphore with 1
    union semun arg;
    arg.val = 1;
    semctl(semafor, 0, SETVAL, arg);

    for (i = 0; i < number_of_processes; i++) {
        if (fork() == 0) {
            struct sembuf op = {0, -1, 0}; //down semaphore
            semop(semafor, &op, 1); // down semaphore
            *sum += i + 1; // add current number to sum
            struct sembuf up_op = {0, 1, 0}; // up semaphore
            semop(semafor, &up_op, 1);
            exit(0); // child process finished, exit
        }
    }

    for (i = 0; i < number_of_processes; i++) 
        wait(NULL); // wait all childs

    printf("%lld\n", *sum); // print final sum
    semctl(semafor, 0, IPC_RMID); // delete all semaphore
    shmdt(sum); // detach shared memory from process
    shmctl(shared_memory, IPC_RMID, NULL); // delete shared memory
    
    return 0;
}
