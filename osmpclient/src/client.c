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

    int rank = -1;
    char bar[] = "hello world!";
    char recv[1024] = "";
    OSMP_Datatype foobar = OSMP_BYTE;
    if((status = OSMP_Size(&rank) != OSMP_SUCCESS))
        printf("Error 1\n");
    printf("size\n");
    if((status = OSMP_Rank(&rank) != OSMP_SUCCESS))
        printf("Error 2\n");
    printf("rank\n");
    if((status = OSMP_Send(bar, sizeof(bar), OSMP_BYTE, rank) != OSMP_SUCCESS))
        printf("Error 3\n");
    printf("send\n");
    if((status = OSMP_Recv(recv, rank, foobar, &rank, &rank) != OSMP_SUCCESS))
        printf("Error 4\n");
    printf("recv\n");
    if(!(status = OSMP_Finalize() == OSMP_SUCCESS))
        printf("Error\n");
    printf("fin\n");
    printf("message is: \"%s\"\n",recv);fflush(NULL);
    
    return 0;
}