#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// //this function make semaphore go down
// void down_semaphore(int semid, int index, int value) {
//     struct sembuf op;
//     op.sem_num = index;
//     op.sem_op = -value;
//     op.sem_flg = 0;
//     semop(semid, &op, 1);
// }

// //this function up all other semaphores
// void up_other_semaphores(int semid, int n, int myIndex) {
//     for (int i = 0; i < n; i++) {
//         if (i == myIndex)
//             continue;
//         struct sembuf op;
//         op.sem_num = i;
//         op.sem_op = 1;
//         op.sem_flg = 0;
//         semop(semid, &op, 1);
//     }
// }

// int main(int argc, char *argv[]) {
//     int limit = atoi(argv[1]); //the limit of numbers to print
//     int n = atoi(argv[2]); //the number of processes

    // //check if the limit is valid
    // if (limit < 0 || limit > 10000) 
    //     return 1;

    // //check if the number of processes is valid
    // if (n <= 0 || n > 500) 
    //     return 1;

//     int SZZZ_sem = semget(IPC_PRIVATE, n, IPC_CREAT | 0666); //create a set of n semaphores

//     if (SZZZ_sem < 0)
//         return 1;

//     //initialize semaphores
//     for (int i = 0; i < n; i++) 
//         semctl(SZZZ_sem, i, SETVAL, n - i - 1);
    
//     //create childs
//     for (int i = 0; i < n; i++) {
//         pid_t pid = fork();
//         if (pid < 0) // fork failed
//             return 1;

//         if (pid == 0) { //child process
//             int current = i + 1;
//             while (current <= limit) { //check if the current number is within the limit
//                 down_semaphore(SZZZ_sem, i, n - 1); //wait on semaphore
//                 printf("%d\n", current); //print current number
//                 fflush(stdout); //print now
//                 current += n; //next number for this process
//                 up_other_semaphores(SZZZ_sem, n, i); //up other semaphores
//             }
//             exit(0);
//         }
//     }

//     //parent wait all childs
//     for (int i = 0; i < n; i++) 
//         wait(NULL);   

//     //delete semaphore
//     semctl(SZZZ_sem, 0, IPC_RMID); 
//     return 0;
// }




// часть 1
int main() {
    printf("Start\n");
    fflush(stdout); //сразу отправить содержимое buffer на экран
    fork();
    printf("Hello\n");
    return 0;
}

// часть 2
int main() {
    pid_t pid = fork(); // pid_t - тип данных для идентификатора процесса (fork возвращает 0 для дочернего процесса и идентификатор дочернего процесса для родительского процесса)

    if (pid == 0) {
        printf("I am child\n");
    } else {
        wait(NULL); //просто жди child, мне не нужен status
        printf("I am parent\n");
    }
    return 0;
} // 0 это child

// часть 3
//Semaphore — это специальная переменная-счётчик, которую используют процессы, чтобы:синхронизироваться
//пример использования: IPC_PRIVATE, чесло симафоров, IPC_CREAT | 0666
//semctl(semid, 0, SETVAL, 5); - установить semaphore номер 0 в значение 5
//semctl(semid, 0, GETVAL); - получить текущее значение semaphore
int main() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 5); // а если эту строку задокументировать, то будет 0
    int value = semctl(semid, 0, GETVAL);
    printf("Semaphore value: %d\n", value);
    return 0;
}

// часть 4
//struct sembuf op; - набор переменных для операции над семафором
//op.sem_num = 0; - номер семафора в наборе
//op.sem_op = -1; - операция (уменьшить на 1)
//op.sem_flg = 0; - флаги (обычно 0)
int main() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 5);

    struct sembuf op; // объявляем структуру для операции над семафором
    op.sem_num = 0; //с каким semaphore работать
    op.sem_op = -2; //что сделать с semaphore, -2 значит уменьшить на 2, и так же наоборот
    op.sem_flg = 0; //0, значит ничего не едлаем (не использовать специальные флаги)

    semop(semid, &op, 1); //эта скрока говорит, что выполни что то что мы указали сверху
    int value = semctl(semid, 0, GETVAL); //получить текущее значение semaphore после операции 

    printf("%d\n", value);
    return 0;
}

// часть 5
int main() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 0);
    pid_t pid = fork(); //создаем новый процесс

    struct sembuf down;
    down.sem_num = 0;
    down.sem_op = -1;
    down.sem_flg = 0;

    struct sembuf up;
    up.sem_num = 0;
    up.sem_op = 1;
    up.sem_flg = 0;

    if (pid == 0) {
        printf("Child waiting\n");
        semop(semid, &down, 1); //процесс засыпает и ожидает, когда можно будет сделать -1
        printf("Child continue\n");
    } else {
        sleep(3); //ждем 3 секунды, чтобы показать, что child действительно ждет и напищет Child waiting
        printf("Parent increase semaphore\n");
        semop(semid, &up, 1); //теперь у нас semaphore стал 1, и child может продолжить работу
        wait(NULL); //ждем, пока child завершится
    }

    return 0;
}

// часть 6
int main() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 0);
    pid_t pid = fork();

    struct sembuf down;
    down.sem_num = 0;
    down.sem_op = -1;
    down.sem_flg = 0;

    struct sembuf up;
    up.sem_num = 0;
    up.sem_op = 1;
    up.sem_flg = 0;

    if (pid == 0) {
        printf("Child waiting\n");
        semop(semid, &down, 1);
        printf("Child continue\n");
    } else {
        sleep(3);
        printf("Parent increase semaphore\n");
        semop(semid, &up, 1);
        wait(NULL);
    }

    return 0;
}


// сейчас часть с Д.З.2
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



//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//H.W.3

//часть 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

int main() {
    // возвращает address memory
    int *shared_number = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //NULL - ОС сама выберет address memory
    //sizeof(int) - сколько памяти выделить
    //PROT_READ | PROT_WRITE - можно читать и писать в эту память
    //MAP_SHARED - эта память будет разделяться между процессами
    //MAP_ANONYMOUS - shared memory не связана с файлом
    //-1 - не используется, так как мы не связываем память с файлом
    //0 - начать с начала memory/file
    *shared_number = 5; //записываем 5 в эту address memory
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child before: %d\n", *shared_number);
        *shared_number = 100;
        printf("Child changed value to: %d\n", *shared_number);
        exit(0);
    }

    wait(NULL);
    printf("Parent sees: %d\n", *shared_number);

    return 0;
}

//часть 2
int main() {
    int *shared_sum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_sum = 0;

    pid_t pid = fork();

    if (pid == 0) {
        *shared_sum += 5;
        printf("Child changed sum to: %d\n", *shared_sum);
        exit(0);
    }
    wait(NULL);
    printf("Parent sees sum: %d\n", *shared_sum);

    return 0;
}

//часть 3
//processes одновременно меняют sum и из-за этого не 197531 или 197532, а 200000
// sem_init(sem, 1, 1); - инициализировать семафор sem
// sem_wait(sem); - заменяет 1 на 0, если стоит сейчас 0 (если был 0, то процесс ждет, а если 1, то он становится 0 и процесс продолжает работу)
// sem_post(sem); - просто увеличивает semaphore на 1 и продолжает работу, он не ждет
int main() {
    //создаем shared memory для семафора
    int *shared_sum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_sum = 0;
    //semaphore создаётся
    //первое значение: 0 - semaphore только внутри одного process, а 1 - semaphore shared между processes
    sem_init(sem, 1, 1); // 1 первая, что семафор будет использоваться между процессами и вторая 1 начальное значение семафора 1

    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            for (int j = 0; j < 100000; j++) {
                sem_wait(sem);
                (*shared_sum)++;
                sem_post(sem);
            }
            exit(0);
        }
    }

    wait(NULL);
    wait(NULL);

    printf("Final sum: %d\n", *shared_sum);
    sem_destroy(sem);

    return 0;
}

//сейчас часть с Д.З.3
int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);

    int *shared_sum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_sum = 0;
    sem_init(sem, 1, 1);

    for (int i = 1; i <= N; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sem_wait(sem);
            *shared_sum += i;
            sem_post(sem);
            exit(0);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Final sum: %d\n", *shared_sum);
    sem_destroy(sem);

    return 0;
}


//Если fork failed -> my fork has failed, and I mean it
// pid_t pid = fork();
// if (pid < 0) {
//     printf("my fork has failed, and I mean it\n");
// }
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

// fork() can fail for different reasons, instruction 4 is wrong
// in task written "need to write "comicsans is awesome" in hebrew" but in instruction 10 written, that we are not alowed
// sum from 1 to 100 = 4950, this is not correct, the answer is 5050
// there are more than 2 wrong instrucrions, therefore instruction 13 is also wrong
// I WANTED TO WRITE YOU A COMMENT IN RUSSIAN FOR YOUR AMUSMENT, AS IT IS NOT FROM RIGHT TO LEFT ;)

int main(int argc, char *argv[]) {
    int namber_of_processes = atoi(argv[argc - 1]), i;

    int *shared_sumber = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *semafor = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *shared_sumber = 0;
    sem_init(semafor, 1, 1); //first 1 - semaphore shared between processes (0 - semaphore only inside one process), second 1 - initial value of semaphore

    for (i = 0; i < namber_of_processes; i++) {
        pid_t pid = fork(); //making new process
        if (pid < 0) { //in the task was written, if there is an issue, then need to print "my fork has failed, and I mean it"
            printf("my fork has failed, and I mean it\n");
            exit(1);
        }

        if (pid == 0) {
            sem_wait(semafor); //continue if semaphore is 1 and decrease it to 0, but if semaphore is 0, then wait until it become 1
            *shared_sumber += i + 1; 
            sem_post(semafor); //increase semaphore on 1
            exit(0); //child process finished
        }
    }

    for (i = 0; i < namber_of_processes; i++) //wait all childs
        wait(NULL); 

    printf("%d\n", *shared_sumber); //print final sum

    return 0;
}


//    mmap и semaphore.h
// в данном моем коде используем mmap и semaphore.h?
// правильно ли все написано?



// павильная домашка 3
// shmget() - создает разделяемую память (shared memory), которое видят все processes
// shmat() - подключает shared memory к process
// shmdt() - отключает shared memory от process
// shmctl() - удаляет shared memory
// semget() - создает набор семафоров
// semctl() - управляет семафорами (установка значения, получение значения, удал)
// semop() - выполняет операции над семафорами (уменьшение, увеличение)

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
    if (number_of_processes <= 0 || number_of_processes > 123456) return 1;

    shared_memory = shmget(IPC_PRIVATE, sizeof(long long), 0666); //create shared memory
    long long *sum = (long long*)shmat(shared_memory, NULL, 0); // attach shared memory to process
    *sum = 0;

    semafor = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    // semctl(semafor, 0, SETVAL, 1); // initialize semaphore with 1
    union semun arg;
    arg.val = 1;
    semctl(semafor, 0, SETVAL, arg);

    for (i = 0; i < number_of_processes; i++) {
        if (fork() ==0) {
            struct sembuf op = {0, -1, 0}; //down semaphore
            semop(semafor, &op, 1); // down semaphore
            *sum += i + 1; // add current number to sum
            struct sembuf up_op = {0, 1, 0}; // up semaphore
            semop(semafor, &up_op, 1);
            exit(0); // child process finished, exit
            
        }
    }

    for (i = 0; i < number_of_processes; i++) 
        wait(NULL); //wait all childs

    printf("%lld\n", *sum); //print final sum
    semctl(semafor, 0, IPC_RMID); //delete all semaphore
    shmdt(sum); // detach shared memory from process
    shmctl(shared_memory, IPC_RMID, NULL); // delete shared memory
    return 0;
}
