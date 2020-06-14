//
//  main.c
//  asd123
//
//  Created by Richard Szczerba on 14/06/2020.
//  Copyright © 2020 Richard Szczerba. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_t threadIDs[N];
pthread_mutex_t	m; 		//initialized to 1
int	state[N];	//initiated to THINKING's
pthread_mutex_t	s[N];		//initialized to 0's
int meal[N]; // initialized to 0

void grab_forks( int philo_id );
void put_away_forks( int philo_id );
void test( int i );

// void* initPhilosopher(void* id);

void* initPhilosopher(void* philo_id) {
  int phil_id = (int)philo_id;
  // philosopher created
  printf("Hello World! It's me, philosopher %d!\n", phil_id);

  while(1) {
    // think a bit
		printf("Philosopher %d is thinking.\n", phil_id);
		sleep(3);

    // eat a bit
    grab_forks(phil_id);
		printf("Philosopher %d is eating.\n", phil_id);
    meal[phil_id] += 1;
		sleep(2);

    put_away_forks(phil_id);
	}
  // leaving this, as best practice
  pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {

  //init m
  pthread_mutex_init(&m, NULL);

  //init s, set to 0
  for (int j=0; j<N; j++) {
    printf("setting s[%d] to 0\n", j);

    int ie = pthread_mutex_init(&s[j], NULL);
    if (ie) {
      printf("ERROR; return code from pthread_mutex_init() is %d\n", ie);
      exit(-1);
    }

    int le = pthread_mutex_lock(&s[j]);
    if (le) {
      printf("ERROR; return code from pthread_mutex_lock() is %d\n", le);
      exit(-1);
    }
  }

  // init philosophers as threads
  int ce;
  int i;
  for (i=0; i<N; i++) {
    printf("In main: creating philosopher %d\n", i);
    ce = pthread_create(&threadIDs[i], NULL, initPhilosopher, (void *)i);
    if (ce) {
      printf("ERROR; return code from pthread_create() is %d\n", ce);
      exit(-1);
    }
  }

  // program execution time
  sleep(10);

  printf("Meal is done.\n");

  for (int j=0; j<N; j++) {
    pthread_cancel(threadIDs[j]);
    pthread_join(threadIDs[j], NULL);

    pthread_mutex_destroy(&s[j]);

    printf("Philosopher %d has been killed.\n", j);
  }

  pthread_mutex_lock(&m);
  pthread_mutex_destroy(&m);

  for (int j=0; j<N; j++) {
    printf("Philosopher %d ate %d meal(s)\n", j, meal[j]);
  }

  pthread_exit(NULL);
  return 0;
}

void grab_forks( int i ) {
  // wait until they can access shared resource
  pthread_mutex_lock( &m );
  state[i] = HUNGRY;
  printf("Philospoher %d is hungry.\n", i);
  test( i );
  // unlock shared resource
  pthread_mutex_unlock( &m );
  // pick up forks
  pthread_mutex_lock( &s[i] );
}

void put_away_forks( int i ) {
  //access shared resource
  pthread_mutex_lock( &m );
  state[i] = THINKING;
  //if possible, let left philosopher eat
  test( LEFT );
  //if possible, let right philosopher eat
  test( RIGHT );
  pthread_mutex_unlock( &m );
}

void test( int i ) {
  // test if philospoher is hungry and both forks are available to him
  if( state[i] == HUNGRY
    && state[LEFT] != EATING
    && state[RIGHT] != EATING )
    {
      // chow time!
      state[i] = EATING;
      // allow to pick up forks
      pthread_mutex_unlock( &s[i] );
    }
  }
