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
#include <pthread.h>
#include <semaphore.h>

#define OSMP_SUCCESS     0
#define OSMP_ERROR      -1

/* max amount of messages per process */
#define OSMP_MAX_MESSAGES_PROC 16
/* max amount of messages per session */
#define OSMP_MAX_SLOTS 256
/* max length of actual message */
#define OSMP_MAX_PAYLOAD_LENGTH 1024

/* use this to determine the message type */
#define OSMP_typeof(X) _Generic(X, \
                            osmp_short:             OSMP_SHORT, \
                            osmp_int:               OSMP_INT, \
                            osmp_long:              OSMP_LONG, \
                            osmp_unsigned_char:     OSMP_UNSIGNED_CHAR, \
                            osmp_unsigned_short:    OSMP_UNSIGNED_SHORT, \
                            osmp_unsigned_int:      OSMP_UNSIGNED, \
                            osmp_unsigned_long:     OSMP_UNSIGNED_LONG, \
                            osmp_float:             OSMP_FLOAT, \
                            osmp_double:            OSMP_DOUBLE, \
                            osmp_byte:              OSMP_BYTE \
                        )

/* provide type info by assigning integer vals to type */
typedef enum {
    OSMP_UNDEFINED,
    OSMP_SHORT,
    OSMP_INT,
    OSMP_LONG,
    OSMP_UNSIGNED_CHAR,
    OSMP_UNSIGNED_SHORT,
    OSMP_UNSIGNED,
    OSMP_UNSIGNED_LONG,
    OSMP_FLOAT,
    OSMP_DOUBLE,
    OSMP_BYTE,
    OSMP_UNDEFINED_UPPER,
} OSMP_Datatype;

/**
 * @brief These are the datatypes which are supported for communication
 * 
 */
typedef short           osmp_short;
typedef int             osmp_int;
typedef long            osmp_long;
typedef unsigned char   osmp_unsigned_char;
typedef unsigned short  osmp_unsigned_short;
typedef unsigned int    osmp_unsigned_int;
typedef unsigned long   osmp_unsigned_long;
typedef float           osmp_float;
typedef double          osmp_double;
typedef char            osmp_byte;

/**
 * @brief Information for async requests, filled and freed by respective 
 *        OSMP_CreateRequest / OSMP_RemoveRequest
 * 
 */
typedef void* OSMP_Request;

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
 * @brief Used to send messages to other child processes
 * 
 * The routine sends a Message to the process number specified
 * by 'dest'. The message contains 'count' elements of type 
 * 'datatype'. The message starts at 'buf'. The call is asynchrounos
 * and thus returns imidiately. It can be waited for with OSMP_Wait().
 * 
 * @param buf Adresse of message
 * @param count Length of message
 * @param datatype Library message datatype
 * @param dest Receiving child process
 * @param request Information about the transfer
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Isend(const void *buf, int count ,OSMP_Datatype datatype, int dest,
               OSMP_Request request);

/**
 * @brief Used to retrieve received messages from other processes
 * 
 * The calling Process receives a message of a maximum of 'count' elements
 * of type 'datatype'. The message is written at 'buf' of the calling process.
 * 'source' contains the number of the child processe which has send the message.
 * 'len' contains the actual length of the message. The call is asynchrounus thus
 * and thus returns imidiately. It can be waited for with OSMP_Wait()
 * 
 * @param buf Adress of message in local memory
 * @param count number of elements
 * @param datatype Library message datatype
 * @param source number of Sender
 * @param len Actual length of message
 * @param request Information about the transfer
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len,
               OSMP_Request request);

/**
 * @brief Used to test the state of the operation linked to request. The routine
 * does not block and returns imidately.
 * 
 * @param request Information about the transfer
 * @param flag Set to 1 if request is finished 0 otherwise
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Test(OSMP_Request request, int *flag);

/**
 * @brief Used to wait for the Operation, linked to request, to finish.
 * 
 * @param request Information about the transfer
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Wait(OSMP_Request request);

/**
 * @brief preparation for asynchrounus communication
 * 
 * @param request The information required for the transfer
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_CreateRequest(OSMP_Request *request);

/**
 * @brief cleanup of request and related information
 * 
 * @param request The request used for the transfer
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_RemoveRequest(OSMP_Request *request);

/**
 * @brief Calling processes are freeing used shared resources
 * 
 * @return int Returns 0 on success, ERROR code otherwise
 */
int OSMP_Finalize(void);
