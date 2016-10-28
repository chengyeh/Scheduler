/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
 Stores information making up a job to be scheduled including any statistics.

 You may need to define some global variables or a struct to store your job queue elements.
 */
typedef struct _job_t {
	int job_number;
	int priority;
	int running_time;

	int arrival_time;
	int start_time;
	int remaining_time;
} job_t;

//Global variables
priqueue_t *job_queue;
scheme_t current_scheduling_scheme;
int total_waiting_time;
int total_turnaround_time;
int total_response_time;
int total_number_of_jobs;

/**
 Initalizes the scheduler.
 
 Assumptions:
 - You may assume this will be the first scheduler function called.
 - You may assume this function will be called once once.
 - You may assume that cores is a positive, non-zero number.
 - You may assume that scheme is a valid scheduling scheme.

 @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
 @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
 */
void scheduler_start_up(int cores, scheme_t scheme) {

	//Set global variables
	current_scheduling_scheme = scheme;
	total_waiting_time = 0;
	total_turnaround_time = 0;
	total_response_time = 0;
	total_number_of_jobs = 0;

	job_queue = malloc(sizeof(priqueue_t));

	switch (scheme) {
	case FCFS:
		priqueue_init(job_queue, compare_FCFS);
		break;
	case SJF:
		priqueue_init(job_queue, compare_SJF);
		break;
	case PSJF:
		priqueue_init(job_queue, compare_PSJF);
		break;
	case PRI:
		priqueue_init(job_queue, compare_PRI);
		break;
	case PPRI:
		priqueue_init(job_queue, compare_PPRI);
		break;
	case RR:
		priqueue_init(job_queue, compare_RR);
		break;
	}
}

/**
 Called when a new job arrives.

 If multiple cores are idle, the job should be assigned to the core with the
 lowest id.
 If the job arriving should be scheduled to run during the next
 time cycle, return the zero-based index of the core the job should be
 scheduled on. If another job is already running on the core specified,
 this will preempt the currently running job.
 Assumptions:
 - You may assume that every job wil have a unique arrival time.

 @param job_number a globally unique identification number of the job arriving.
 @param time the current time of the simulator.
 @param running_time the total number of time units this job will run before it will be finished.
 @param priority the priority of the job. (The lower the value, the higher the priority.)
 @return index of core job should be scheduled on
 @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority) {
	//Create struct object and populate the members
	job_t *new_job;
	new_job = malloc(sizeof(job_t));

	new_job->job_number = job_number;
	new_job->running_time = running_time;
	new_job->priority = priority;

	new_job->arrival_time = time;
	new_job->remaining_time = running_time;

	//Set global variables
	total_number_of_jobs++;

	//Gather info job queue info
	job_t* peek_job = priqueue_at(job_queue, 0);

	//******delete********
	printf(
	ANSI_COLOR_YELLOW"******** scheduler_new_job: Elements in jobs queue : ");
	for (int i = 0; i < priqueue_size(job_queue); i++)
		printf("%d ", *((int *) priqueue_at(job_queue, i)));
	printf("*********"ANSI_COLOR_RESET"\n");
	//^^^^^^delete^^^^^^^^

	//Main decision to schedule
	if (current_scheduling_scheme == FCFS) {
		priqueue_offer(job_queue, new_job);
		if (peek_job == NULL) {
			new_job->start_time = time;
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

/**
 Called when a job has completed execution.

 The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
 If any job should be scheduled to run on the core free'd up by the
 finished job, return the job_number of the job that should be scheduled to
 run on core core_id.

 @param core_id the zero-based index of the core where the job was located.
 @param job_number a globally unique identification number of the job.
 @param time the current time of the simulator.
 @return job_number of the job that should be scheduled to run on core core_id
 @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time) {

	//******delete********
	printf(
			ANSI_COLOR_MAGENTA"******** core_id: %d, job_number: %d, time: %d *********\n",
			core_id, job_number, time);
	printf(
			ANSI_COLOR_MAGENTA"******** scheduler_job_finished: Elements in jobs queue : ");
	for (int i = 0; i < priqueue_size(job_queue); i++)
		printf("%d ", *((int *) priqueue_at(job_queue, i)));
	printf("*********"ANSI_COLOR_RESET"\n");
	//^^^^^^delete^^^^^^^^

	//Get info about the job finished
	job_t* finished_job = priqueue_poll(job_queue);

	//Calculate metrics
	total_turnaround_time += time - finished_job->arrival_time;
	total_response_time += finished_job->start_time - finished_job->arrival_time;
	total_waiting_time += finished_job->start_time - finished_job->arrival_time;
	free(finished_job);

	job_t* peek_job = priqueue_at(job_queue, 0);
	if (peek_job == NULL) {
		return -1;
	} else {
		peek_job->start_time = time;
		return peek_job->job_number;
	}
}

/**
 When the scheme is set to RR, called when the quantum timer has expired
 on a core.

 If any job should be scheduled to run on the core free'd up by
 the quantum expiration, return the job_number of the job that should be
 scheduled to run on core core_id.

 @param core_id the zero-based index of the core where the quantum has expired.
 @param time the current time of the simulator.
 @return job_number of the job that should be scheduled on core cord_id
 @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time) {
	return -1;
}

/**
 Returns the average waiting time of all jobs scheduled by your scheduler.

 Assumptions:
 - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
 @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time() {
	return ((float) (total_waiting_time) / total_number_of_jobs);
}

/**
 Returns the average turnaround time of all jobs scheduled by your scheduler.

 Assumptions:
 - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
 @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time() {
	return ((float) (total_turnaround_time) / total_number_of_jobs);
}

/**
 Returns the average response time of all jobs scheduled by your scheduler.

 Assumptions:
 - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
 @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time() {
	return ((float) (total_response_time) / total_number_of_jobs);
}

/**
 Free any memory associated with your scheduler.

 Assumptions:
 - This function will be the last function called in your library.
 */
void scheduler_clean_up() {
	//priqueue_destroy(job_queue);
}

/**
 This function may print out any debugging information you choose. This
 function will be called by the simulator after every call the simulator
 makes to your scheduler.
 In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

 2(-1) 4(0) 1(-1)

 This function is not required and will not be graded. You may leave it
 blank if you do not find it useful.
 */
void scheduler_show_queue() {

}

int compare_FCFS(const void* a, const void* b) {
	return (((job_t*) a)->arrival_time - ((job_t*) b)->arrival_time);
}

int compare_SJF(const void* a, const void* b) {
	return (((job_t*) a)->running_time - ((job_t*) b)->running_time);
}

int compare_PSJF(const void* a, const void* b) {

	int compare = ((job_t*) a)->running_time - ((job_t*) b)->running_time;
	if (compare == 0) {
		compare = ((job_t*) a)->arrival_time - ((job_t*) b)->arrival_time;
	}
	return (compare);
}

int compare_PRI(const void* a, const void* b) {
	return (((job_t*) a)->priority - ((job_t*) b)->priority);
}

int compare_PPRI(const void* a, const void* b) {

	int compare = ((job_t*) a)->priority - ((job_t*) b)->priority;
	if (compare == 0) {
		int compare = ((job_t*) a)->arrival_time - ((job_t*) b)->arrival_time;
	}
	return (compare);
}

int compare_RR(const void* a, const void* b) {
	return 1;
}
