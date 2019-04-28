/**
 * @file queue.h
 * 
 * @brief The source for the program which gets multithreaded
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date April 2019
 */

#include "osmplib.h"

/**
 * @brief Operates on OSMP_queue, appends node to queue
 * 
 * @param node object to be appended
 * @param queue queue to append to
 */
void push(OSMP_msg_node *node, OSMP_queue *queue);

/**
 * @brief Operates on OSMP_queue, pulls node from queue
 * 
 * @param queue queue to pop from
 * @return OSMP_msg_node pointer to popped object
 */
OSMP_msg_node *pop(OSMP_queue *queue);

/**
 * @brief initialize queue
 * 
 * @param queue init this
 * @return int Returns 0 on success, ERROR code otherwise
 */
int init_queue(OSMP_queue *queue, unsigned int max_length);

/**
 * @brief deinitializes the queue
 * 
 * @param queue destroy this
 * @return int Returns 0 on success, ERROR code otherwise
 */
int destroy_queue(OSMP_queue *queue);
