#include "osmplib.h"
#include <semaphore.h>

typedef struct {
    void *shm_base;
    int shm_fd;
    int rank;
} OSMP_globals;

extern OSMP_globals osmp_globals;

/*
 * useful defines that turn up places 
 * also this is the actual memory layout 
 */
#define base ((OSMP_base*)osmp_globals.shm_base)
#define pcb_list ((OSMP_pcb*)(((char*)osmp_globals.shm_base)+sizeof(OSMP_base)))


typedef struct {
    int next;
    int sender;
    int receiver;
    int len;
    char msg_buf[OSMP_MAX_PAYLOAD_LENGTH];
    OSMP_Datatype datatype;
} OSMP_msg_node;

typedef struct {
    int front;
    int back;
    sem_t slots_available;
    sem_t slots_in_use;
    sem_t queue_lock;
} OSMP_queue;

typedef struct {
    size_t shm_size;
    unsigned int num_proc;
    OSMP_msg_node messages[OSMP_MAX_SLOTS];
    OSMP_queue empty_list;
} OSMP_base;

typedef struct {
    OSMP_queue inbox;
    pid_t pid;
} OSMP_pcb;

typedef struct {
    void **recv_buf;
    const void **send_buf;
    int *count;
    OSMP_Datatype *datatype;
    int *dest;
    int **source;
    int **len;
    OSMP_Request *request;
} OSMP_async_arglist;

size_t OSMP_sizeof(OSMP_Datatype dt);
