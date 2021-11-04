/*
 * UVic CSC 360, Summer 2021
 * This code copyright 2021: Roshan Lasrado, Mike Zastre
 *
 * Assignment 2: Task 1
 * --------------------
 * Simulate a Vaccination Center with `1` Registration Desk and `N` 
 * Vaccination Stations.
 * 
 * Input: Command Line args
 * ------------------------
 * ./vaccine <num_vaccination_stations `N`> <input_test_case_file>
 * e.g.
 *      ./vaccine 10 test1.txt
 * 
 * Input: Test Case file
 * ---------------------
 * Each line corresponds to person arrive for vaccinationn 
 * and is formatted as:
 *
 * <person_id>:<arrival_time>,<service_time>
 * 
 * NOTE: All times represented in `Tenths of a Second`.
 * 
 * Expected Sample Output:
 * -----------------------
 * Person 1: Arrived at 3.
 * Person 1: Added to the queue.
 * Vaccine Station 1: START Person 1 Vaccination.
 * Vaccine Station 1: FINISH Person 1 Vaccination.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>


/*
 * A queue structure provided to you by the teaching team. This header
 * file contains the function prototypes; the queue routines are
 * linked in from a separate .o file (which is done for you via
 * the `makefile`).
 */
#include "queue.h"


/* 
 * Some compile-time constants related to assignment description.
 */
#define MAX_VAC_STATIONS 10
#define MAX_INPUT_LINE 100
#define TENTHS_TO_SEC 100000


/*
 * Here are variables that are available to all threads in the
 * process. Given these are global, you need not pass them as
 * parameters to functions. However, you must properly initialize
 * the queue, the mutex, and the condition variable.
 */
Queue_t *queue;
pthread_mutex_t queue_mutex;
pthread_cond_t queue_condvar;
unsigned int num_vac_stations;
unsigned int is_vac_completed = false;


/*
 * Function: reg_desk
 * ------------------
 *  Registration Desk Thread.
 *  Reads the input file and adds the vaccination persons to the
 *  queue as per their arrival times.
 * 
 *  arg: Input file name
 *  
 *  returns: null
 */
void *reg_desk(void *arg) {

    char *filename = (char *)arg;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "File \"%s\" does not exist.\n", filename);
        exit(1);
    }

    char line[MAX_INPUT_LINE];
    unsigned int current_time = 0;

    while (fgets(line, sizeof(line), fp)) {
        int person_id;
        int person_arrival_time;
        int person_service_time;

        int vars_read = sscanf(line, "%d:%d,%d", &person_id, 
            &person_arrival_time, &person_service_time);

        if (vars_read == EOF || vars_read != 3) {
            fprintf(stderr, "Error reading from the file.\n");
            exit(1);
        }

        if (person_id < 0 || person_arrival_time < 0 || 
            person_service_time < 0)
        {
            fprintf(stderr, "Incorrect file input.\n");
            exit(1);
        }

        int arrival_time = person_arrival_time;

        // Sleep to simulate the persons arrival time.
        usleep((arrival_time - current_time) * TENTHS_TO_SEC);
        fprintf(stdout, "Person %d: arrived at time %d.\n", 
            person_id, arrival_time);

        // Update the current time based on simulated time elapsed.
        current_time = arrival_time;

        // TODO ... Insert your Code Here
        // For what to implement, please refer to the function
        // description above. Beware that you are now working in a 
        // multi-threaded scenario.

        pthread_mutex_lock(&queue_mutex);
	struct PersonInfo* p = new_person();
	p->id = person_id;
        p->arrival_time = person_arrival_time;
        p->service_time = person_service_time;
	while (queue_size(queue)==MAX_INPUT_LINE){
		pthread_cond_wait(&queue_condvar,&queue_mutex);
	}
	
	enqueue(queue, p);
	fprintf(stdout, "Person %d: Added to the queue.\n",person_id);
	pthread_mutex_unlock(&queue_mutex);
	pthread_cond_signal(&queue_condvar);// when a person enter the queue, wake up a waiting thread.
	
    }

    fclose(fp);

    // TODO ... Insert your Code Here
    // Notify all waiting threads that the vaccination drive is now 
    // completed.

    is_vac_completed=true;
    pthread_cond_broadcast(&queue_condvar);  // finish reading the file, wake up all waiting thread.
    return NULL;
    
}


/*
 * Function: vac_station
 * ---------------------
 *  Vaccination Station Thread.
 *  Vaccinate the persons from the queue as per their service times.
 *
 *  arg: Vaccination station number
 *
 *  returns: null
 *
 * Remember: When performing a vaccination, the vac_station 
 * must sleep for the period of time required to "service"
 * that "person". (This is part of the simulation). Assuming
 * the "person" to be serviced is a pointer to an instance of
 * PersonInfo, the sleep would be something like:
 *
 *      usleep(person->service_time * TENTHS_TO_SEC);
 *
 */
void *vac_station(void *arg) {

    int station_num = *((int *)arg);
    free(arg);

    while (true) {
        // TODO ... Insert your Code Here
        // For what to implement, please refer to the function
        // description above and the assignment description.

	pthread_mutex_lock(&queue_mutex);
	   
	while(is_empty(queue)){  
     	    if (is_vac_completed){ // if all people get vaccined, then dont wait.  
		pthread_cond_signal(&queue_condvar);
		pthread_mutex_unlock(&queue_mutex);
		return NULL;
	    }	
            pthread_cond_wait(&queue_condvar,&queue_mutex); 
	}
	struct PersonInfo* person = dequeue(queue);
	pthread_cond_signal(&queue_condvar);
	pthread_mutex_unlock(&queue_mutex);
    	fprintf(stdout,"Vaccine Station %d: START Person %d Vaccination.\n",station_num,person->id) ;
    	usleep(person->service_time * TENTHS_TO_SEC );
    	fprintf(stdout, "Vaccine Station %d: FINISH Person %d Vaccination.\n",station_num,person->id);
    }

    return NULL;
    
}


/*
 * Function: validate_args
 * -----------------------
 *  Validate the input command line args.
 *
 *  argc: Number of command line arguments provided
 *  argv: Command line arguments
 */
void validate_args(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of input args provided! "
                        "Expected 2, received %d\n", 
                        argc - 1);
        exit(1);
    }

    num_vac_stations = atoi(argv[1]);
    if ((num_vac_stations <= 0) || (num_vac_stations > MAX_VAC_STATIONS)) {
        fprintf(stderr, "Vaccine stations exceeded the MAX LIMIT.%d\n", 
            argc - 1);
        exit(1);
    }

}

/*
 * Function: initialize_vars
 * -------------------------
 *  Initialize the mutex, conditional variable and the queue.
 */
void initialize_vars() {
    // TODO ... Insert your Code Here
    // For what to implement, please refer to the function
    // description above.
    queue = init_queue();
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_condvar, NULL);
}


/*
 * Function: main
 * -------------------------
 * argv[1]: Number of Vaccination Stations 
 * argv[2]: Input file/test case.
 */
int main(int argc, char *argv[]) {
    int i, status;

    validate_args(argc, argv);

    initialize_vars();
    
    // TODO ... Insert your Code Here
    // 1. Create threads.
    // 2. Wait for threads to complete.
    // 3. Clean up.
  
    pthread_t th[num_vac_stations+1];

    for (i=0; i< (num_vac_stations+1); i++){
	int* a = malloc(sizeof(int));
	*a=i;
	if (i==0){
	    if (pthread_create(&th[i],NULL,&reg_desk, (void *) argv[2])!=0){
		perror("Failed to create thread\n");
	    }
	    
	}else {
	    if (pthread_create(&th[i],NULL,&vac_station, a)!=0){
		perror("Failed to create thread\n");
	    }	
	}
    }
    for(int j=0; j< (num_vac_stations+1); j++){
        if(pthread_join(th[j],NULL)!=0){
	    perror("Failed to join thread\n");
	}	 
    }

    
    deallocate(queue);// free queue 
    pthread_mutex_destroy(&queue_mutex);// clean up mutex and condition variable
    pthread_cond_destroy(&queue_condvar);

   
    return 0;
    
}

