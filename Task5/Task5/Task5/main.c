/* The sleeping barber problem */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define BARBERSCOUNT_M 1
#define BARBERSCOUNT_F 1
#define BARBERSCOUNT_MF 1
#define CHAIRS 5

#define CUSTOMERS 0
#define BARBERS_M 1
#define BARBERS_F 2
#define BARBERS_MF 3
#define MUTEX 4

#define CUSTOMER_M 1
#define CUSTOMER_F 2

int sem_id;
int *waiting;
int *count;
struct sembuf semaphore;

void up(int sem_id,int sem_num,struct sembuf *semaphore) {
    semaphore->sem_num=sem_num;
    semaphore->sem_op=1;
    semaphore->sem_flg=0;
    semop(sem_id,semaphore,1);
    
}

void down(int sem_id,int sem_num,struct sembuf *semaphore) {
    semaphore->sem_num=sem_num;
    semaphore->sem_op=-1;
    semaphore->sem_flg=0;
    semop(sem_id,semaphore,1);
}

void initSem(int sem_id,int sem_num,int val) {
    
    union semnum {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        
    } argument;
    argument.val=val;
    
    semctl(sem_id,sem_num,SETVAL,argument);
    
}

int main() {
    
    //    shm_id=shmget(shm_key,sizeof(int),IPC_CREAT|0666);
    sem_id = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
    waiting = mmap(NULL, sizeof *waiting, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    count = mmap(NULL, sizeof *count, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    
    *waiting = 0;
    *count = 0;
    
    initSem(sem_id, CUSTOMERS, 0);
    initSem(sem_id, BARBERS_M, BARBERSCOUNT_M);
    initSem(sem_id, BARBERS_F, BARBERSCOUNT_F);
    initSem(sem_id, BARBERS_MF, BARBERSCOUNT_MF);
    initSem(sem_id, MUTEX, 1);
    
    printf("There are %d chairs.\n",CHAIRS);
    
    int pid = 0;
    for (int i = 0; i<3; i++) {
        pid = fork ();
        if (pid != 0) {
            break;
        }
    }

    if(pid != 0) {
        printf("%d\n", pid);
        while(1) {
            down(sem_id, CUSTOMERS, &semaphore);
            down(sem_id, MUTEX, &semaphore);
            *waiting = *waiting - 1;
            up(sem_id, BARBERS_M, &semaphore);
            up(sem_id, MUTEX, &semaphore);
            printf("The barber %d is now cutting hair.\n", pid);
            sleep(6);
        }
    }
    else {
        // customer
        while(1) {
            sleep(1);
            down(sem_id, MUTEX, &semaphore);
            if(*waiting < CHAIRS) {
                *count = *count + 1;
                printf("Customer %d is seated.\n", *count);
                *waiting = *waiting + 1;
                up(sem_id, CUSTOMERS, &semaphore);
                up(sem_id, MUTEX, &semaphore);
            }
            else {
                *count = *count + 1;
                printf("Customer %d left the shop.\n", *count);
                up(sem_id, MUTEX, &semaphore);
            }
        }
    }
    
    
}
