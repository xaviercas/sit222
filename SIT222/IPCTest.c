/*
* Week 6 Interprocess communication
*
* gcc -o IPCTest IPCTest.c
*/

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define NUM_SEMAPHORES 2
#define NUM_ITERATIONS 5
#define SHM_SIZE sizeof(int)

#define IPCKEYFILE "/dev/null"
#define IPCPROJID 12345

// down / wait
/**
 * [SemWait description]
 * @param  semKey [Semaphore set]
 * @param  semNum [semaphore number in set]
 * @return   
 * semop     [
    A positive integer increments the semaphore value by that amount.
    A negative integer decrements the semaphore value by that amount. An attempt to set a semaphore to a value less than zero fails or blocks, depending on whether IPC_NOWAIT is in effect.
    A value of zero means to wait for the semaphore value to reach zero. ]
    The process blocks (unless the IPC_NOWAIT flag is set), and remains blocked until:
    the semaphore operations can all finish, so the call succeeds,
    the process receives a signal, or the semaphore set is removed. 
 */

int SemWait(key_t semKey, int semNum)
{

	struct sembuf semops[2];

        //      semKey      |     semNum
    // ----------------------------------
    // semaphore set    => semaphore [0]
    //                  => semaphore [1]
    //                  
    // semops[n]
    // sem_num;  /* semaphore number */
    // sem_op;   /* semaphore operation */
    // sem_flg;  /* operation flags */

    semops[0].sem_num = semNum; // semaphore to perform op on
    semops[0].sem_op = 0; // Wait for semaphore semval to become 0
    semops[0].sem_flg = 0; // Allow block


    semops[1].sem_num = semNum; // semaphore to perform op on
    semops[1].sem_op = 1; // Increment semaphore semval
    semops[1].sem_flg = 0; // Allow block

	// int semop(int semid, struct sembuf *sops, unsigned nsops); 
	// semop() performs operations on selected semaphores in the set indicated by semid. 
	// Each of the nsops elements in the array pointed to by sops specifies an operation to 
	// be performed on a single semaphore. The elements of this structure are of type struct 
	// sembuf, containing the following members:
	// unsigned short sem_num;  /* semaphore number */
	// short          sem_op;   /* semaphore operation */
	// short          sem_flg;  /* operation flags */

    if(semop(semKey, semops, 2) == -1)
    {
      perror("semop(SemWait)");
      return -1;
  }
  usleep(1);
  return 0;
}

int SemSignal(key_t semKey, int semNum)
{
    struct sembuf semops;

    semops.sem_num = semNum; /* select semaphore */
    semops.sem_op = -1; /* do a wait operation */
    semops.sem_flg = 0; /* allow block */

    if(semop(semKey, &semops, 1) == -1)
    {
        perror("semop(SemSignal)");
        return(-1);
    }
    usleep(1);
    return(0);
}

int main()
{
    key_t semKey, shmKey;
    int semID;
    int sems[2];
    int shmID;
    int count;
    int child;
    int *data;
    struct shmid_ds shmdealloc;

    // Create keys for semaphores and shared memory
    // ftok  - convert a pathname and a project identifier to a System V IPC key
    // A process-shared semaphore must be placed in a shared memory region
    if((semKey = ftok(IPCKEYFILE, IPCPROJID)) == -1)
    {
        perror("ftok(sem)");
        return(1);
    }
    if((shmKey = ftok(IPCKEYFILE, IPCPROJID)) == -1)
    {
        perror("ftok(shm)");
        return(1);
    }

    /* Create NUM_SEMAPHORE (2) semaphores */
    if((semID = semget(semKey, NUM_SEMAPHORES, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    {
        perror("semget");
        return(1);
    }

    /* Initialise the semaphores to 1 */
    sems[0] = 0;
    sems[1] = 0;
    if(semctl(semID, 1, SETALL, sems) == -1)
    {
        perror("semctl(init_sems)");
        return(1);
    }

    // shmget() returns the identifier of the System V shared memory segment
    //    associated with the value of the argument key.  A new shared memory
    //    segment, with size equal to the value of size rounded up to a
    //    multiple of PAGE_SIZE, is created if key has the value IPC_PRIVATE or
    //    key isn't IPC_PRIVATE, no shared memory segment corresponding to key
    //    exists, and IPC_CREAT is specified in shmflg.

    /* Create a shared memory region */
    if((shmID = shmget(shmKey, SHM_SIZE, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
    {
        perror("shmget");
        return(1);
    }

    // shmat()
    //    shmat() attaches the System V shared memory segment identified by
    //    shmid to the address space of the calling process.  The attaching
    //    address is specified by shmaddr with one of the following criteria:

    //    *  If shmaddr is NULL, the system chooses a suitable (unused) page-
    //       aligned address to attach the segment.

    //    *  If shmaddr isn't NULL and SHM_RND is specified in shmflg, the
    //       attach occurs at the address equal to shmaddr rounded down to the
    //       nearest multiple of SHMLBA.

    //    *  Otherwise, shmaddr must be a page-aligned address at which the
    //       attach occurs.

    if((data = (int *)shmat(shmID, NULL, 0)) == NULL)
    {
        perror("shmat");
        return(1);
    }

    *data = 0;

    printf("Press ENTER to begin...");
    getchar();

    /* Fork a second process */
    if((child = fork()) == -1)
    {
        perror("fork");
        return(1);
    }
    else if(child > 0)
    {
        /* Parent */
        printf("Parent entering loop...\n");
        for(count = 0 ; count < NUM_ITERATIONS ; count++)
        {
            SemWait(semID, 0);
            SemWait(semID, 1);
            printf("Parent: Data is %d\n", *data);
            SemSignal(semID, 0);
            SemSignal(semID, 1);
        }
        printf("Parent finished loop.\n");
        wait(); /* Wait for the child to exit */
    }
    else
    {
        /* Child */
        srand(time(0));
        printf("Child entering loop...\n");
        for(count = 0 ; count < NUM_ITERATIONS ; count++)
        {
            SemWait(semID, 0);
            SemWait(semID, 1);
            printf("Child: Data is now %d\n", *data = rand());
            SemSignal(semID, 0);
            SemSignal(semID, 1);
        }
        printf("Child finished loop.\n");
        return(0); /* Exit the child */
    }

    /* Tidy up */
    printf("Press ENTER to tidy up...");
    getchar();
    if(semctl(semID, 1, IPC_RMID) == -1)
    {
        perror("semctl");
        return(1);
    }
    if(shmdt(data) == -1)
    {
        perror("shmdt");
        return(1);
    }
    if(shmctl(shmID, IPC_RMID, &shmdealloc) == -1)
    {
        perror("shmctl");
        return(1);
    }
}