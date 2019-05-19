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

OSMP_globals osmp_globals = {.shm_base = NULL};

/* pragmas are to shut the compiler up about unused argc and argv */
#pragma GCC diagnostic ignored "-Wunused-parameter"
int OSMP_Init(int *argc, char ***argv)
{
#pragma GCC diagnostic pop

    char *shm_name = getenv("OSMPSHM");
    osmp_globals.shm_fd = shm_open(shm_name, O_RDWR, 0644);
    if(osmp_globals.shm_fd == -1)
        return OSMP_ERROR;

    osmp_globals.shm_base = mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED, osmp_globals.shm_fd, 0);
    if(osmp_globals.shm_base == MAP_FAILED)
        return OSMP_ERROR;
    size_t shm_size = *(size_t *)osmp_globals.shm_base;
    munmap(osmp_globals.shm_base,sizeof(size_t));

    osmp_globals.shm_base = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, osmp_globals.shm_fd, 0);
    if(osmp_globals.shm_base == MAP_FAILED)
        return OSMP_ERROR;

    int size;
    int ret = OSMP_Size(&size);
    if(ret == OSMP_ERROR)
        return OSMP_ERROR;
    pid_t pid = getpid();

    for(int i = 0; i < size; i++) {
        if(pid == pcb_list[i].pid) {
            osmp_globals.rank = i;
            return OSMP_SUCCESS;
        }
    }

    OSMP_Finalize();
    return OSMP_ERROR;
}

int OSMP_Finalize(void)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_SUCCESS;

    if(munmap(osmp_globals.shm_base, *(size_t *)osmp_globals.shm_base) == -1)
        return OSMP_ERROR;
    
    osmp_globals.shm_base = NULL;

    return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;

    *rank = osmp_globals.rank;
    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;

    *size = base->num_proc;
    return OSMP_SUCCESS;
}

size_t OSMP_sizeof(OSMP_Datatype dt)
{
    switch(dt) {
        case OSMP_SHORT:             return sizeof(OSMP_SHORT);
        case OSMP_INT:               return sizeof(OSMP_INT);
        case OSMP_LONG:              return sizeof(OSMP_LONG);
        case OSMP_UNSIGNED_CHAR:     return sizeof(OSMP_UNSIGNED_CHAR);
        case OSMP_UNSIGNED_SHORT:    return sizeof(OSMP_UNSIGNED_SHORT);
        case OSMP_UNSIGNED:          return sizeof(OSMP_UNSIGNED);
        case OSMP_UNSIGNED_LONG:     return sizeof(OSMP_UNSIGNED_LONG);
        case OSMP_FLOAT:             return sizeof(OSMP_FLOAT);
        case OSMP_DOUBLE:            return sizeof(OSMP_DOUBLE);
        case OSMP_BYTE:              return sizeof(OSMP_BYTE);
        default: printf("Not an OSMP_Datatype!\n"); return(OSMP_ERROR);
   }
}