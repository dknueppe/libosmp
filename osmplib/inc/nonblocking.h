/**
 * @file nonblocking.h
 * 
 * @brief Header for helper functions and such...
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date May 2019
 */

typedef enum {
    async_trans_complete,
    async_trans_prepared,
    async_trans_incomplete
} OSMP_thread_state;

int send_wrapper(void *arglist);

int recv_wrapper(void *arglist);
