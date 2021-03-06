/**
 * @file queue.c
 * 
 * @brief The osmplib header 
 * 
 * @author Daniel Knüppe
 * 
 * @date April 2019
 */
#include <semaphore.h>
#include "queue.h"

void push(OSMP_msg_node *node, OSMP_queue *queue)
{
    /* make sure not to push more than the queue can (or should) handle */
    sem_wait(&queue->slots_available);
    /* lock further access to queue */
    sem_wait(&queue->queue_lock);

    /* get index for messages[] in shm from node */
    unsigned int index = node - base->messages;
    /* check if queue is empty */
    if((queue->back == queue->front) && ( queue->back == -1))
        queue->front = queue->back = index;
    node->next = -1;
    base->messages[queue->back].next = index;
    queue->back = index;

    /* tell the queue that another node is available / has been pushed to it */
    sem_post(&queue->slots_in_use);
    /* unlock access to the queue */
    sem_post(&queue->queue_lock);
}

OSMP_msg_node *pop(OSMP_queue *queue)
{
    /* make sure the queue is not empty / wait for it to get a new node */
    sem_wait(&queue->slots_in_use);
    /* lock further access to queue */
    sem_wait(&queue->queue_lock);

    unsigned int index = queue->front;
    queue->front = base->messages[queue->front].next;

    /* tell the queue that another node has been popped */
    sem_post(&queue->slots_available);
    /* unlock access to the queue */
    sem_post(&queue->queue_lock);

    return &base->messages[index];
}

int init_queue(OSMP_queue* queue, unsigned int slots_available)
{
    if(sem_init(&queue->slots_available, 1, slots_available))
        return(OSMP_ERROR);
    if(sem_init(&queue->slots_in_use, 1, 0))
        return(OSMP_ERROR);
    if(sem_init(&queue->queue_lock, 1, 1))
        return(OSMP_ERROR);
    return OSMP_SUCCESS;
}

int destroy_queue(OSMP_queue *queue)
{
    if(sem_destroy(&queue->slots_available))
        return(OSMP_ERROR);
    if(sem_destroy(&queue->slots_in_use))
        return(OSMP_ERROR);
    if(sem_destroy(&queue->queue_lock))
        return(OSMP_ERROR);
    return OSMP_SUCCESS;
}
