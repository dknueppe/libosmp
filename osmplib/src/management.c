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

void push(OSMP_msg_node *node, OSMP_queue *queue)
{
    /* lock further access to queue */
    sem_wait(&queue->queue_lock);
    /* make sure not to push more than the queue can (or should) handle */
    sem_wait(&queue->max_length);

    /* get index for messages[] in shm from node */
    unsigned int index = node - ((OSMP_base *)g_shm)->messages;
    /* check if queue is empty */
    if((queue->back == queue->front) && ( queue->back == -1))
        queue->front = queue->back = index;
    /* will assert if queue is corrupted */
    //assert(((OSMP_base *)g_shm)->messages[queue->back].next != -1);
    node->next = -1;
    ((OSMP_base *)g_shm)->messages[queue->back].next = index;
    queue->back = index;

    /* tell the queue that another node is available / has been pushed to it */
    sem_post(&queue->availabe);
    /* unlock access to the queue */
    sem_post(&queue->queue_lock);
}

OSMP_msg_node *pop(OSMP_queue *queue)
{
    /* lock further access to queue */
    sem_wait(&queue->queue_lock);
    /* make sure the queue is not empty / wait for it to get a new node */
    sem_wait(&queue->availabe);

    unsigned int index = queue->front;
    queue->front = ((OSMP_base *)g_shm)->messages[queue->front].next;

    /* tell the queue that another node has been popped */
    sem_post(&queue->max_length);
    /* unlock access to the queue */
    sem_post(&queue->queue_lock);
    return &((OSMP_base *)g_shm)->messages[index];
}
