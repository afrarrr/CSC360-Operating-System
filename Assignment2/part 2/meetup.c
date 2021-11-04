/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */

int num;
int count;
int meet;

resource_t r;

sem_t turnstile1;
sem_t turnstile2;
sem_t mutex; // protect count variable
sem_t barrier;

//follows Pg 41 Reusable barrier solution in the book "LittleBookOfSemaphores"


void initialize_meetup(int n, int mf) {
    char label[100];
    

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
     num = n;
     meet = mf;
     count = 0;
     init_resource(&r, "initialize...");
     
     // inistialize the first is locked 
     if (sem_init(&turnstile1, 0, 0) != 0) {
         fprintf(stderr, "Failed to initialize semaphore 'turnstile1'\n");
         exit(1);
     }
     // initialize the second is open
     if (sem_init(&turnstile2, 0, 1) != 0) {
         fprintf(stderr, "Failed to initialize semaphore 'turnstile2'\n");
         exit(1);
     }
     if (sem_init(&barrier, 0, num) != 0) {
         fprintf(stderr, "Failed to initialize semaphore 'barrier'\n");
         exit(1);
     }
     if (sem_init(&mutex, 0, 1) != 0) {
         fprintf(stderr, "Failed to initialize semaphore 'mutex'\n");
         exit(1);
     }
}


void join_meetup(char *value, int len) {

    // wait for the current group to fill
    sem_wait(&barrier);
  
    sem_wait(&mutex);// protect the count variable

    count+=1;    
    if(count==1){
	// if meetfirst, then record the first person's code. 
        if (meet == MEET_FIRST) {
            write_resource(&r, value, len);	             
        }
    }
    if (count == num) {
	// if meetlast, then record the last perosn's code. 
        if (meet == MEET_LAST) {
            write_resource(&r, value, len);
        }
        // when n people (all thread) arrives, we lock the second and unlock the first
        sem_wait(&turnstile2);// lock the second
        sem_post(&turnstile1);// unlock the first
    }
    sem_post(&mutex);

    sem_wait(&turnstile1);
    sem_post(&turnstile1);

    sem_wait(&mutex); 
    // share the codeword with the group 
    read_resource(&r, value, len);
    count--; 

    if (count == 0) {
    	sem_wait(&turnstile1); //lock the first
    	sem_post(&turnstile2); // unlock the second
    }    
    sem_post(&mutex);

    sem_wait(&turnstile2);//second turnstile
    sem_post(&turnstile2);

    //prepare for the next group
    sem_post(&barrier);
    
}

