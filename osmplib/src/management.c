/**
 * @file management.c
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
#include <assert.h>
#include "queue.h"
#include "osmplib.h"

void *g_shm = NULL;
int g_shm_fd = 0;

int OSMP_Init(int *argc, char ***argv)
{
    char *shm_name = getenv("OSMPSHM");
    g_shm_fd = shm_open(shm_name, O_RDWR, 0644);
    if(g_shm_fd == -1)
        return OSMP_ERROR;

    g_shm = mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED, g_shm_fd, 0);
    if(g_shm == MAP_FAILED)
        return OSMP_ERROR;
    size_t shm_size = *(size_t *)g_shm;
    munmap(g_shm,sizeof(size_t));

    g_shm = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, g_shm_fd, 0);
    if(g_shm == MAP_FAILED)
        return OSMP_ERROR;

    return OSMP_SUCCESS;
}

int OSMP_Finalize(void)
{
    if(g_shm == NULL)
        return OSMP_SUCCESS;

    if(munmap(g_shm, *(size_t *)g_shm) == -1)
        return OSMP_ERROR;
    
    g_shm = NULL;

    return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{
    if(g_shm == NULL)
        return OSMP_ERROR;

    int size;
    int ret = OSMP_Size(&size);
    if(ret == OSMP_ERROR)
        return OSMP_ERROR;
    pid_t pid = getpid();

    for(int i = 0; i < size; i++) {
        if(pid == pcb_list[i].pid) {
            *rank = i;
            return OSMP_SUCCESS;
        }
    }

    return OSMP_ERROR;
}

int OSMP_Size(int *size)
{
    if(g_shm == NULL)
        return OSMP_ERROR;

    *size = ((OSMP_base *)g_shm)->num_proc;
    return OSMP_SUCCESS;
}

size_t OSMP_sizeof(OSMP_Datatype dt)
{
    switch(dt) {
        case OSMP_SHORT_TI: return sizeof(OSMP_SHORT);
        case OSMP_INT_TI: return sizeof(OSMP_INT);
        case OSMP_LONG_TI: return sizeof(OSMP_LONG);
        case OSMP_UNSIGNED_CHAR_TI: return sizeof(OSMP_UNSIGNED_CHAR);;
        case OSMP_UNSIGNED_SHORT_TI: return sizeof(OSMP_UNSIGNED_SHORT);
        case OSMP_UNSIGNED_TI: return sizeof(OSMP_UNSIGNED);
        case OSMP_UNSIGNED_LONG_TI: return sizeof(OSMP_UNSIGNED_LONG);
        case OSMP_FLOAT_TI: return sizeof(OSMP_FLOAT);
        case OSMP_DOUBLE_TI: return sizeof(OSMP_DOUBLE);
        case OSMP_BYTE_TI: return sizeof(OSMP_BYTE);
        default: printf("Not an OSMP_Datatype!\n"); exit(1);
   }
}