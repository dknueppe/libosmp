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
#include <stdlib.h>
#include "osmplib.h"

int main(int argc, char *argv[])
{
    for(int i = 0; i < argc; i++)
        printf("%s ",argv[i]);
    printf("\n");
    int status;
    if(!(status = OSMP_Init(&argc, &argv) == OSMP_SUCCESS))
        printf("Error\n");

    int rank = -1;
    char bar[] = "hello world!";
    char recv[1024] = "";
    OSMP_Datatype foobar = OSMP_typeof(bar[0]);
    if((status = OSMP_Size(&rank) != OSMP_SUCCESS))
        printf("Error 1\n");
    if((status = OSMP_Rank(&rank) != OSMP_SUCCESS))
        printf("Error 2\n");
    if((status = OSMP_Send(bar, sizeof(bar), OSMP_typeof(bar[0]), rank) != OSMP_SUCCESS))
        printf("Error 3\n");
    if((status = OSMP_Recv(recv, 13, foobar, &rank, &rank) != OSMP_SUCCESS))
        printf("Error 4\n");
    if((status = OSMP_Finalize() != OSMP_SUCCESS))
        printf("Error\n");
    printf("message is: \"%s\"\n",recv);
    return 0;
}