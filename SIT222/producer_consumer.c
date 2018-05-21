/*
* @Author: xavier
* @Date:   2018-05-09 11:29:52
* @Last Modified by:   xavier
* @Last Modified time: 2018-05-21 23:04:59
*
* Week 6 Interprocess communication
*
* Producer adds items to a shared buffer
* Consummer 'consumes' items from the shared buffer
* Problem: Interaction between producer and consumer could come to a deadlock
* because of system interrupts noccuring unpredictably.
* Solution: Protect critical section of both consumer(s) and producer
* with semaphore.
* 
* Compile:
* 
* -o output file
* 
* -pthread
* Define additional macros required for using the POSIX threads library. 
* You should use this option consistently for both compilation and linking. 
* This option is supported on GNU/Linux targets,  most other Unix 
* derivatives, and also on x86 Cygwin and MinGW targets.
* 
* gcc -pthread -o producer_consumer producer_consumer.c
*/

#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#define PROCESS_SHARED 1
#define MAX_ITEMS 10
#define MIN_ITEMS 0
#define MTX_INIT 1
#define MAX_RUN 100
#define SITEM_COUNT "/item_count"
#define SEMPTY_COUNT "/empty_count"
#define SMUTEX "/mutex"

int cleanup();
int delete_sems();
void getinfo();

sem_t *empty_count, *item_count, *mutex;

void producer() {
	int c, i = 0;
	while(i < MAX_RUN){
		sem_wait(empty_count);
		sem_wait(mutex);
		sem_getvalue(empty_count, &c);
		printf("Producer %d added 1 item, total is now %d\n", getpid(), (MAX_ITEMS - c));
		sem_post(mutex);
		sem_post(item_count);
		i = i + 1;
		usleep(1);
	}
}

void consummer(){
	int c, i = 0;
	while(i < MAX_RUN){
		sem_wait(item_count);
		sem_wait(mutex);
		sem_getvalue(item_count, &c);
		printf("Consummer %d removed 1 item, total is now %d\n", getpid(), c);
		sem_post(mutex);
		sem_post(empty_count);
		i = i + 1;
		usleep(1);
	}
}

int main(int argc, char const *argv[])
{
	int process;

	// Cleanup on ubuntu
	delete_sems();

	// Creates three named semaphores
	empty_count = sem_open(SEMPTY_COUNT, O_CREAT, 0666, MAX_ITEMS);
	item_count = sem_open(SITEM_COUNT, O_CREAT, 0666, MIN_ITEMS);
	mutex = sem_open(SMUTEX, O_CREAT, 0666, MTX_INIT);

	// Create child process
	if((process = fork()) == -1)
	{
		perror("fork");
		return(1);
	}

	if (process > 0){
		// Parent
		producer();
		// Wait for child to terminate
		wait();
	} else {
		// Child
		empty_count = sem_open(SEMPTY_COUNT, 0);
		item_count = sem_open(SITEM_COUNT, 0);
		mutex = sem_open(SMUTEX, 0);
		consummer();
		// Terminates
		return(0);
	} 
	cleanup();
	delete_sems();
    // Ends main
	return 0;
}

int delete_sems(){
	if (
		remove("/dev/shm/sem.empty_count") == -1 ||
		remove("/dev/shm/sem.item_count") == -1 ||
		remove("/dev/shm/sem.mutex") == -1
		) 
	{
		// No worries
		return 0;
	}
	return 0;
}

int cleanup(){
	// Clean up
	printf("Cleaning up...");
	if (
		sem_close(empty_count) == -1 || 
		sem_close(item_count) == -1 ||
		sem_close(mutex) == -1
		)
	{
		perror("sem_close");
		return(1);
	}
	return 0;
}
