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
#include "osmplib.h"

void *g_shm = NULL;
int g_shm_fd = 0;

int OSMP_Init(int *argc, char ***argv)
{
    g_shm_fd = shm_open(*argv[0], O_RDWR, 0644);
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
        return OSMP_ERROR;

    if(munmap(g_shm, *(size_t *)g_shm) == -1)
        return OSMP_ERROR;

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

    while(size){
        if(pid == ((pid_t *)g_shm + ((OSMP_base *)g_shm)->pid_list)[size]){
            *rank = size;
            return OSMP_SUCCESS;
        }
        --size;
    }

    return OSMP_SUCCESS;
}

int OSMP_Size(int *size)
{
    if(g_shm == NULL)
        return OSMP_ERROR;

    *size = ((OSMP_base *)g_shm)->num_proc;
    return OSMP_SUCCESS;
}

int push(OMSP_msg_node *node, OSMP_queue *queue)
{
    /* will assert if queue is corrupted */
    assert(((OSMP_base *)g_shm)->messages[queue->back].next == 0);
    node->next = 0;
    unsigned int temp = node - ((OSMP_base *)g_shm)->messages;
    ((OSMP_base *)g_shm)->messages[queue->back].next = temp;
    queue->back = temp;
}

OMSP_msg_node *pop(OSMP_queue *queue)
{
    unsigned int temp = queue->front;
    queue->front = ((OSMP_base *)g_shm)->messages[queue->front].next;
    return &((OSMP_base *)g_shm)->messages[temp];
}
