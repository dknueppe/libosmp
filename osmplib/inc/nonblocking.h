/**
 * @file nonblocking.h
 * 
 * @brief Header for helper functions and such...
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date May 2019
 */

extern OSMP_globals osmp_globals;

typedef enum {
    async_trans_complete,
    async_trans_prepared,
    async_trans_incomplete
} OSMP_thread_state;

void *send_wrapper(void *arglist);

void *recv_wrapper(void *arglist);
