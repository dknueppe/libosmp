/**
 * @file client.c
 * 
 * @brief The source for the program which gets multithreaded
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date April 2019
 */

#include <stdio.h>
#include <unistd.h>
#include "osmplib.h"

int main(int argc, char *argv[])
{
    int status;
    if(!(status = OSMP_Init(&argc, &argv) == OSMP_SUCCESS))
        printf("Error\n");
    printf("shared memory message :\n%s\n", (char*)g_shm + sizeof(struct shm_info));
    sleep(2);
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