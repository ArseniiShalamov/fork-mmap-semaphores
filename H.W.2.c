#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int i = 0, j = 0;
    int get_count = atoi(argv[1]);
    int get_processes = atoi(argv[2]);

    //check if the get_count is valid
    if (get_count < 0 || get_count > 10000) 
        return 1;

    //check if the get_processes of processes is valid
    if (get_processes <= 0 || get_processes > 500) 
        return 1;

    int semid = semget(IPC_PRIVATE, get_processes, IPC_CREAT | 0666);

    for (i = 0; i < get_processes; i++) 
        semctl(semid, i, SETVAL, get_processes - i - 1); //a0 = 4, a1 = 3, a2 = 2, a3 = 1, a4 = 0

    for (i = 0; i < get_processes; i++) {
        pid_t pid = fork(); //making new process
        
        if (pid == 0) { //working in child process
            int num_to_print = i + 1;
            while (num_to_print <= get_count) { 
                struct sembuf op = {i, -(get_processes - 1), 0}; //down semaphore
                // struct sembuf op;
                // op.sem_num = i;
                // op.sem_op = -(get_processes - 1);
                // op.sem_flg = 0;
                semop(semid, &op, 1);
                printf("%d\n", num_to_print); //print current number
                fflush(stdout); //print now on the screen

                for (j = 0; j < get_processes; j++) { //up other semaphores on +1
                    if (j != i) {
                        struct sembuf up_op = {j, 1, 0};
                        semop(semid, &up_op, 1);
                    }   
                }
                num_to_print += get_processes; //next number for this process
            }
            exit(0); //child process finished, exit
        }
    }
    for (i = 0; i < get_processes; i++) 
        wait(NULL); //wait all childs
    
    semctl(semid, 0, IPC_RMID); //delete all semaphore

    return 0;
}