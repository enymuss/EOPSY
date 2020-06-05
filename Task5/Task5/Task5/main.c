/* The sleeping barber problem */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>

#define BARBERSCOUNT_M 1 // (N1)
#define BARBERSCOUNT_F 1 // (N2)
#define BARBERSCOUNT_MF 1 // (N3)
#define CHAIRS 5 // (M)
#define SUMBARBERS BARBERSCOUNT_M+BARBERSCOUNT_F+BARBERSCOUNT_MF

// semaphore indexes
#define CUSTOMERS 0
#define BARBERS_M 1
#define BARBERS_F 2
#define BARBERS_MF 3
#define MUTEX 4

// customer types
#define CUSTOMER_M 1
#define CUSTOMER_F 2

// barber types
#define BARBER_M CUSTOMER_M
#define BARBER_F CUSTOMER_F
#define BARBER_MF 3

int sem_id;
int *waiting;
int *count;
struct sembuf semaphore;

// fifo function helpers on waiting queue
void printQueue(int* intArray, int length);
int popFirstItem (int* intArray, int length);
int popItemAtIndex (int* intArray, int length, int index);
int indexOfFirstEmptyChair (int* intArray, int length);
int indexOfFirstCustomerType(int* intArray, int length, int customerType);
int addCustomerToQueue (int* intArray, int length, int customerType);

// semaphore operations
void up(int sem_id,int sem_num,struct sembuf *semaphore) {
    semaphore->sem_num = sem_num;
    semaphore->sem_op = 1;
    semaphore->sem_flg = 0;
    semop(sem_id, semaphore, 1);
}

void down(int sem_id,int sem_num,struct sembuf *semaphore) {
    semaphore->sem_num = sem_num;
    semaphore->sem_op = -1;
    semaphore->sem_flg = 0;
    semop(sem_id, semaphore, 1);
}

// create semaphore with value
void initSem(int sem_id,int sem_num,int val) {
    union semnum {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        
    } argument;
    argument.val = val;
    
    semctl(sem_id, sem_num, SETVAL, argument);
}

int main() {
    // create a set of 5 semaphores
    sem_id = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
    // create queue and customer count which can be shared between processes
    waiting = mmap(NULL, sizeof(int)*CHAIRS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    count = mmap(NULL, sizeof *count, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    
    // at the start, queue is empty and there where no customers
    *waiting = 0;
    *count = 0;
    
    // set semaphore and their initial value
    initSem(sem_id, CUSTOMERS, 0);
    initSem(sem_id, BARBERS_M, BARBERSCOUNT_M);
    initSem(sem_id, BARBERS_F, BARBERSCOUNT_F);
    initSem(sem_id, BARBERS_MF, BARBERSCOUNT_MF);
    // mutual exclusion set to 1
    initSem(sem_id, MUTEX, 1);
    
    printf("There are %d chairs.\n", CHAIRS);
    
    int pid = 0;
    int barberType = 0;
    int semaphoreIndex = 0;
    // create barbers and set the types of customers they serve
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
                // BARBER_MF can serve any customer, so take the first one
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
                // check if the queue has customers the barber can serve
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
            // check if there are empty chairs
            int index = indexOfFirstEmptyChair(waiting, CHAIRS);
            if(index != -1) {
                // add random customer
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
    // remove item at index and move all items after it 1 position forward
    // set the end to empty
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
