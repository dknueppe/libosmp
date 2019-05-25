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
    int error = 0;
    int rank = -1;
    int size = -1;
    int source = -1;
    int len = -1;
    int sender = -1;
    osmp_byte bar[] = "hello world!";
    char recv[OSMP_MAX_PAYLOAD_LENGTH] = "";
    OSMP_Datatype foobar = OSMP_typeof(bar[0]);
    OSMP_Request myrequest1, myrequest2;
    
    /* print out all arguments */
    printf("argv[] = ");
    for(int i = 0; i < argc; i++)
        printf("%s ",argv[i]);
    printf("\n");fflush(NULL);

    /* small nonexhaustive test of blocking functions */
    if((status = OSMP_Init(&argc, &argv) != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Size(&size) != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Rank(&rank) != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Send(bar, sizeof(bar), OSMP_typeof(bar[0]), rank) != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Recv(recv, 13, foobar, &source, &len) != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    printf("message from blocking is:\t\"%s\"\n", recv);

    /* small nonexhaustive test of nonblocking functions */
    char irecv[OSMP_MAX_PAYLOAD_LENGTH] = "";
    if((status = OSMP_CreateRequest(&myrequest1)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_CreateRequest(&myrequest2)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;

    printf("Sendargs in front of functioncall: buf = 0x%08X, count = %d, dt = %d, dest = %d\n",
            (unsigned int)bar, sizeof(bar), OSMP_typeof(*bar), rank);
    if((status = OSMP_Isend(bar, sizeof(bar), OSMP_typeof(bar[0]), rank, myrequest1)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Irecv(irecv, 13, OSMP_typeof(*bar), &sender, &len, myrequest2)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;

    if((status = OSMP_Wait(myrequest1)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_Wait(myrequest2)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;

    if((status = OSMP_RemoveRequest(&myrequest1)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;
    if((status = OSMP_RemoveRequest(&myrequest2)) != OSMP_SUCCESS)
        printf("Error %d\n", error);fflush(NULL);error++;

    printf("message from nonblocking is:\t\"%s\"\n", irecv);fflush(NULL);

    if((status = OSMP_Finalize() != OSMP_SUCCESS))
        printf("Error %d\n", error);fflush(NULL);error++;
    return 0;
}