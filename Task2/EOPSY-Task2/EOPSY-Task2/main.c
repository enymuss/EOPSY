//
//  main.c
//  EOPSY-Task2
//
//  Created by Richard Szczerba on 17/04/2020.
//  Copyright © 2020 Richard Szczerba. All rights reserved.
//

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "stdbool.h"

#define NUM_CHILD 3

#ifdef WITH_SIGNALS
#define WITHSIGNALS 1
#else
#define WITHSIGNALS 0
#endif

void child(void);
void parent(int pid);

void ignoreHandler(int sig);
void terminationHandler(int sig);

bool keyboardInterrupt = false;

int main(int argc, const char * argv[]) {
    
    pid_t childArray[NUM_CHILD] = {0};
    pid_t forkValue = 0;
    pid_t callerID = getpid();
    
    for (int i = 0; i<NUM_CHILD; i++) {
        forkValue = fork();
        if (keyboardInterrupt) {
            forkValue = -1;
        }
        sleep(1);
        
        switch (forkValue) {
            case -1:
                // error
                if (keyboardInterrupt) {
                    printf("parent[%d]: Interrupt of creation process", getpid());
                } else {
                    printf("parent[%d]: Failed to create child process", getpid());
                }
                
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
                if (WITHSIGNALS) {
                    struct sigaction new_sa;
                    struct sigaction old_sa;
                    sigfillset(&new_sa.sa_mask);
                    new_sa.sa_handler = SIG_IGN;
                    new_sa.sa_flags = 0;

                    for (int j = 0; j<32; j++) {
                        if (j != SIGCHLD) {
                            if (sigaction(j, &new_sa, &old_sa) == 0 && old_sa.sa_handler != SIG_IGN)
                            {
                                new_sa.sa_handler = ignoreHandler;
                                sigaction(SIGINT, &new_sa, 0);
                            }
                        }
                    }
                }
                waitpid(forkValue, NULL, 0);
                break;
        }
    }
    if (getpid() == callerID) {
        printf("parent[%d]: All child process created\n", getpid());
        
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
        printf("parent[%d]: There are no more child process\n", getpid());
        printf("parent[%d]: Count exit codes: %d\n", getpid(), countTerminations);
    }
    
    if (WITHSIGNALS) {
        // restore handlers.
        for (int i=0; i<32; i++) {
            signal(i, SIG_DFL);
        }
    }
    
    
    
    return 0;
}


void child(void) {
    if (WITHSIGNALS) {
        struct sigaction new_sa;
        sigfillset(&new_sa.sa_mask);
        new_sa.sa_flags = 0;

        new_sa.sa_handler = terminationHandler;
        sigaction(SIGTERM, &new_sa, NULL);
        sigaction(SIGTERM, NULL, NULL);
    }
    
    
    printf("child[%d]: Parent PID: %d\n", getpid(), getppid());
    sleep(10);
    printf("child[%d]: Execution completed\n", getpid());
    // exit(0)
}

void ignoreHandler(int sig) {
    printf("You've used control-c. Info int: %d\n", sig);
    keyboardInterrupt = true;
}

void terminationHandler(int sig) {
    printf("This process is terminated");
}
