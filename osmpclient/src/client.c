#include <stdio.h>
#include "osmplib.h"

/* This program manages the launch of executables that want to utilize 
 * the osmp library 
 */

void *shm;

int main(int argc, char *argv[])
{
    int status;
    if(!(status = OSMP_Init(&argc, &argv) == OSMP_SUCCESS))
        printf("Error\n");
    printf("shared memory message :\n%s\n", (char*)shm + sizeof(struct shm_info));
    sleep(5);
    if(!(status = OSMP_Finalize() == OSMP_SUCCESS))
        printf("Error\n");

    int foo = 0;
    char bar[] = "hello world!";
    OSMP_Datatype foobar = NULL;
    if(!(status = OSMP_Size(&foo) == OSMP_SUCCESS))
        printf("Error\n");
    if(!(status = OSMP_Rank(&foo) == OSMP_SUCCESS))
        printf("Error\n");
    if(!(status = OSMP_Send(bar,foo, foobar, foo) == OSMP_SUCCESS))
        printf("Error\n");
    if(!(status = OSMP_Recv(bar, foo, foobar, &foo, &foo) == OSMP_SUCCESS))
        printf("Error\n");
    
    return 0;
}