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
    int status;
    int rank = -1;
    osmp_byte bar[] = "hello world!";
    char recv[1024] = "";
    OSMP_Datatype foobar = OSMP_typeof(bar[0]);
    OSMP_Request myrequest;
    
    /* print out all arguments */
    printf("argv[] = ");
    for(int i = 0; i < argc; i++)
        printf("%s ",argv[i]);
    printf("\n");

    /* small nonexhaustive test of blocking functions */
    if((status = OSMP_Init(&argc, &argv) != OSMP_SUCCESS))
        printf("Error\n");
    if((status = OSMP_Size(&rank) != OSMP_SUCCESS))
        printf("Error 1\n");
    if((status = OSMP_Rank(&rank) != OSMP_SUCCESS))
        printf("Error 2\n");
    if((status = OSMP_Send(bar, sizeof(bar), OSMP_typeof(bar[0]), rank) != OSMP_SUCCESS))
        printf("Error 3\n");
    if((status = OSMP_Recv(recv, 13, foobar, &rank, &rank) != OSMP_SUCCESS))
        printf("Error 4\n");
    if((status = OSMP_Finalize() != OSMP_SUCCESS))
        printf("Error 5\n");
    printf("message from blocking is:\t\"%s\"\n", recv);

    /* small nonexhaustive test of nonblocking functions */
    char irecv[OSMP_MAX_PAYLOAD_LENGTH] = "";
    int sender;
    int len;
    if((status = OSMP_CreateRequest(&myrequest)) != OSMP_SUCCESS)
        printf("Error 6\n");
    if((status = OSMP_Isend(bar, sizeof(bar), OSMP_typeof(bar[0]), rank, myrequest)) != OSMP_SUCCESS)
        printf("Error 7\n");
    if((status = OSMP_Irecv(irecv, 13, OSMP_typeof(*bar), &sender, &len, myrequest)) != OSMP_SUCCESS)
        printf("Error 8\n");
    if((status = OSMP_RemoveRequest(&myrequest)) != OSMP_SUCCESS)
        printf("Error 9\n");
    printf("message from nonblocking is:\t\"%s\"\n", irecv);

    return 0;
}