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
#define SUMBARBERS BARBERSCOUNT_M+BARBERSCOUNT_F+BARBERSCOUNT_MF

#define CUSTOMERS 0
#define BARBERS_M 1
#define BARBERS_F 2
#define BARBERS_MF 3
#define MUTEX 4

#define CUSTOMER_M 1
#define CUSTOMER_F 2

#define BARBER_M CUSTOMER_M
#define BARBER_F CUSTOMER_F
#define BARBER_MF 3

int sem_id;
int *waiting;
int *count;
struct sembuf semaphore;

int popFirstItem (int* intArray, int length);
int addCustomerToQueue (int* intArray, int length, int customerType);
int indexOfFirstEmptyChair (int* intArray, int length);
void printQueue(int* intArray, int length);
int indexOfFirstCustomerType(int* intArray, int length, int customerType);
int popItemAtIndex (int* intArray, int length, int index);

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
    waiting = mmap(NULL, sizeof(int)*CHAIRS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
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
    int barberType = 0;
    int semaphoreIndex = 0;
    
    for (int i = 0; i<(SUMBARBERS); i++) {
        pid = fork ();
        if (pid != 0) {
            if (i < BARBERSCOUNT_M) {
                barberType = BARBER_M;
            } else if (i - BARBERSCOUNT_M < BARBERSCOUNT_F) {
                barberType = BARBER_F;
            } else {
                barberType = BARBER_MF;
            }
            
            break;
        }
    }
    
    if(pid != 0) {
        switch (barberType) {
            case BARBER_M:
                semaphoreIndex = BARBERS_M;
                break;
                
            case BARBER_F:
                semaphoreIndex = BARBERS_F;
                break;
                
            case BARBER_MF:
                semaphoreIndex = BARBER_MF;
                break;
                
            default:
                printf("Unknown barber type: %d", barberType);
                break;
        }
        if (barberType == BARBER_MF) {
            while(1) {
                down(sem_id, CUSTOMERS, &semaphore);
                down(sem_id, MUTEX, &semaphore);
                int customerType = popFirstItem(waiting, CHAIRS);
                up(sem_id, semaphoreIndex, &semaphore);
                up(sem_id, MUTEX, &semaphore);
                printf("barber[%d]: type %d is now cutting hair of customer: %d\n", pid, barberType, customerType);
                sleep(6);
            }
        } else {
            while(1) {
                down(sem_id, CUSTOMERS, &semaphore);
                down(sem_id, MUTEX, &semaphore);
                int index = indexOfFirstCustomerType(waiting, CHAIRS, barberType);
                if (index >= 0) {
                    int customerType = popItemAtIndex(waiting, CHAIRS, index);
                    up(sem_id, semaphoreIndex, &semaphore);
                    up(sem_id, MUTEX, &semaphore);
                    up(sem_id, CUSTOMERS, &semaphore);
                    printf("barber[%d]: type %d is now cutting hair of customer: %d\n", pid, barberType, customerType);
                    sleep(6);
                } else {
                    printf("barber[%d]: type %d has no customers and is going to sleep\n", pid, barberType);
                    up(sem_id, MUTEX, &semaphore);
                }
            }}
    }
    else {
        // customer
        while(1) {
            sleep(1);
            down(sem_id, MUTEX, &semaphore);
            int index = indexOfFirstEmptyChair(waiting, CHAIRS);
            if(index != -1) {
                *count = *count + 1;
                printf("Customer %d is seated.\n", *count);
                //                *waiting = *waiting + 1;
                addCustomerToQueue(waiting, CHAIRS, (rand()%2)+1);
                printQueue(waiting, CHAIRS);
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

int popFirstItem (int* intArray, int length) {
    return popItemAtIndex(intArray, length, 0);
}

int popItemAtIndex (int* intArray, int length, int index) {
    int itemValue = intArray[index];
    
    for (int i = index; i < length-1; i++) {
        intArray[i] = intArray[i+1];
    }
    intArray[length-1] = 0;
    
    return itemValue;
}

int indexOfFirstEmptyChair (int* intArray, int length) {
    return indexOfFirstCustomerType(intArray, length, 0);
}

int indexOfFirstCustomerType(int* intArray, int length, int customerType) {
    for (int i = 0; i<length; i++) {
        if (intArray[i] == customerType ) {
            return i;
        }
    }
    return -1;
}

int addCustomerToQueue (int* intArray, int length, int customerType) {
    int insertIndex = indexOfFirstEmptyChair(intArray, length);
    if (insertIndex >= 0) {
        intArray[insertIndex] = customerType;
        return 0;
    }
    else {
        return -1;
    }
}

void printQueue(int* intArray, int length){
    for (int i = 0; i<length; i++) {
        printf("array[%d]: %d\n", i, intArray[i]);
    }
    printf("\n");
}
