#include "osmplib.h"
#include <semaphore.h>

/*
 * useful defines that turn up places 
 * also this is the actual memory layout 
 */
#define base ((OSMP_base*)g_shm)
#define pcb_list ((OSMP_pcb*)(((char*)g_shm)+sizeof(OSMP_base)))

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
    sem_t max_length;
    sem_t availabe;
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

size_t OSMP_sizeof(OSMP_Datatype dt);
