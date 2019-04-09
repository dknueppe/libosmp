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

int OSMP_Init(int *argc, char ***argv)
{
    printf("OSMP_Init was called!\n");

    extern void *shm;
    shm_meta.shm_fd = shm_open(*argv[0], O_RDWR, 0644);
    if(shm_meta.shm_fd == -1)
        return OSMP_ERROR;
    shm = mmap(NULL, sizeof(struct shm_info), PROT_READ | PROT_WRITE, MAP_SHARED, shm_meta.shm_fd, 0);
    if(shm == MAP_FAILED)
        return OSMP_ERROR;

    struct shm_info* tmp = shm;
    shm_meta.shm_size = tmp->shm_size;    
    //strcpy(shm_meta.shm_name, tmp->shm_name);
    munmap(shm,sizeof(struct shm_info));
    shm = mmap(NULL, shm_meta.shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_meta.shm_fd, 0);
    if(shm == MAP_FAILED)
        return OSMP_ERROR;

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

    extern void *shm;
    if(munmap(shm, shm_meta.shm_size) == -1)
        return OSMP_ERROR;
    printf("the unlinking goes wrong\n");
    if(shm_unlink(shm_meta.shm_name) == -1)
        return OSMP_ERROR;

    return OSMP_SUCCESS;
}
