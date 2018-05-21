/*
* Week 4 Memory
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    char *ptr;
    unsigned long memsize = 0;

    printf("How much memory should be allocated? ");
    scanf("%ld", &memsize);

    printf("Allocating memory...");
    fflush(stdout);
    // malloc allocate a block of mem on the heap
    // accessed via a pointer
    if((ptr = (char *)malloc(memsize)) == NULL)
        printf("Failed.\n");
    else
    {
        printf("Success! Address: 0x%08x\n", (unsigned int)ptr);

        printf("Initialise memory (Y/N)? ");
        getchar(); /* go past new line */
        if(toupper(getchar()) == 'Y')
        {
            printf("Initialising memory...");
            fflush(stdout);
            for(i = 0 ; i < memsize ; i++)
                ptr[i] = (char)(i % 256);
            printf("Done!\n");
        }

        printf("Deallocate memory (Y/N)? ");
        getchar(); /* go past new line */
        if(toupper(getchar()) == 'Y')
        {
            printf("Deallocating memory...");
            // Release the block of memory allocated
            free(ptr);
            printf("Done!\n");
        }
    }

    printf("Enter any character and press ENTER to terminate...\n");
    getchar(); /* go past new line */
    getchar();
}