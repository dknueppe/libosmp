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
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "osmplib.h"

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
