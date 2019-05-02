/**
 * @file osmplib.h
 * 
 * @brief The osmplib header 
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date April 2019
 */

#pragma once

#include <semaphore.h>

#define OSMP_SUCCESS     0
#define OSMP_ERROR      -1

/* max amount of messages per process */
#define OSMP_MAX_MESSAGES_PROC 16
/* max amount of messages per session */
#define OSMP_MAX_SLOTS 256
/* max length of actual message */
#define OSMP_MAX_PAYLOAD_LENGTH 1024

extern void *g_shm; 
extern int g_shm_fd;

typedef void* OSMP_Request;

typedef enum {
    OSMP_SHORT = 1,
    OSMP_INT,
    OSMP_LONG,
    OSMP_UNSIGNED_CHAR,
    OSMP_UNSIGNED_SHORT,
    OSMP_UNSIGNED,
    OSMP_UNSIGNED_LONG,
    OSMP_FLOAT,
    OSMP_DOUBLE,
    OSMP_BYTE,
} OSMP_Datatype;

/**
 * @brief initializes the OSMP environment
 * 
 * This routine initializes the OSMP environment and grants the child
 * processes access to shared resources.
 * 
 * @param argc Adresse of argument counter
 * @param argv Adresse of argument vector
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Init(int *argc, char ***argv);

/**
 * @brief Info about number of child processes
 * 
 * @param size number of child processes
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Size(int *size);

/**
 * @brief Info about the number of the calling child process
 * 
 * @param rank Number of calling child process
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Rank(int *rank);

/**
 * @brief Used to send messages to other child processes
 * 
 * The routine sends a Message to the process number specified
 * by 'dest'. The message contains 'count' elements of type 
 * 'datatype'. The message starts at 'buf'. The call is synchrounus
 * thus it blocks the process until the message is copied.
 * 
 * @param buf Adresse of message
 * @param count Length of message
 * @param datatype Library message datatype
 * @param dest Receiving child process
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest);

/**
 * @brief Used to retrieve received messages from other processes
 * 
 * The calling Process receives a message of a maximum of 'count' elements
 * of type 'datatype'. The message is written at 'buf' of the calling process.
 * 'source' contains the number of the child processe which has send the message.
 * 'len' contains the actual length of the message. The call is synchrounus thus
 * it blocks the process until the message is copied.
 * 
 * @param buf Adress of message in local memory
 * @param count number of elements
 * @param datatype Library message datatype
 * @param source number of Sender
 * @param len Actual length of message
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len);

/**
 * @brief calling processes are freeing used shared resources
 * 
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Finalize(void);
