//
//  main.c
//  EOPSY-Task2
//
//  Created by Richard Szczerba on 17/04/2020.
//  Copyright © 2020 Richard Szczerba. All rights reserved.
//

// test synchronization mechanisms and signals.
// Use the following system functions: fork(), wait(), signal() or sigaction() and kill().

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM_CHILD 3

void child(void);
void parent(int pid);

int main(int argc, const char * argv[]) {
    
    pid_t childArray[NUM_CHILD] = {0};
    pid_t forkValue = 0;
    pid_t callerID = getpid();
    
    for (int i = 0; i<NUM_CHILD; i++) {
        forkValue = fork();
        sleep(1);
        
        switch (forkValue) {
            case -1:
                // error
                printf("Failed to create child process");
                // kill child process
                for (int i = 0; i<NUM_CHILD; i++) {
                    if (childArray[i] > 0) {
                        kill(childArray[i], SIGTERM);
                    }
                }
                exit(1);
                break;
            case 0:
                child();
                break;
                
            default:
                childArray[i] = forkValue;
                wait(NULL);
                break;
        }
    }
    if (getpid() == callerID) {
        printf("All child process created\n");
        
    }
//    3. Print a message about creation of all child processes.
    pid_t terminatedChildProcess;
    int countTerminations = 0;
    for (int i=0; i<NUM_CHILD; i++) {
        terminatedChildProcess = wait(NULL);
        if (terminatedChildProcess != -1) {
            countTerminations++;
        }
    }
    
    if (getpid() == callerID) {
        printf("There are no more child process\n");
        printf("Count exit codes: %d\n", countTerminations);
    }
    
    return 0;
}


void child(void) {
    printf("Parent PID: %d\n", getppid());
    sleep(5);
    printf("Execution completed\n");
    // exit(0)
}
