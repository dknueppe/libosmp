/**
 * @file queue.c
 * 
 * @brief The osmplib header 
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date April 2019
 */
#include <semaphore.h>
#include "queue.h"
#include "osmplib.h"

void push(OSMP_msg_node *node, OSMP_queue *queue)
{
    /* lock further access to queue */
    sem_wait(&queue->queue_lock);
    /* make sure not to push more than the queue can (or should) handle */
    sem_wait(&queue->max_length);

    /* get index for messages[] in shm from node */
    unsigned int index = node - base->messages;
    /* check if queue is empty */
    if((queue->back == queue->front) && ( queue->back == -1))
        queue->front = queue->back = index;
    node->next = -1;
    base->messages[queue->back].next = index;
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
    queue->front = base->messages[queue->front].next;

    /* tell the queue that another node has been popped */
    sem_post(&queue->max_length);
    /* unlock access to the queue */
    sem_post(&queue->queue_lock);
    return &base->messages[index];
}

int init_queue(OSMP_queue* queue, unsigned int max_length)
{
    if(sem_init(&queue->max_length, 1, max_length))
        return(OSMP_ERROR);
    if(sem_init(&queue->availabe, 1, 0))
        return(OSMP_ERROR);
    if(sem_init(&queue->queue_lock, 1, 1))
        return(OSMP_ERROR);
    return OSMP_SUCCESS;
}

int destroy_queue(OSMP_queue *queue)
{
    if(sem_destroy(&queue->max_length))
        return(OSMP_ERROR);
    if(sem_destroy(&queue->availabe))
        return(OSMP_ERROR);
    if(sem_destroy(&queue->queue_lock))
        return(OSMP_ERROR);
    return OSMP_SUCCESS;
}