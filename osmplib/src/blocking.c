/**
 * @file blocking.c
 * 
 * @brief The osmplib header 
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date April 2019
 */

#include <stdio.h>
#include "osmplib.h"

int OSMP_Init(int *argc, char ***argv)
{
    printf("OSMP_Init was called!\n");
    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    printf("OSMP_Size was called!\n");
    return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{
    printf("OSMP_Rank was called!\n");
    return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    printf("OSMP_Send was called!\n");
    return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    printf("OSMP_Recv was called!\n");
    return OSMP_SUCCESS;
}

int OSMP_Finalize(void)
{
    printf("OSMP_Finalize was called!\n");
    return OSMP_SUCCESS;
}
