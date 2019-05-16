/**
 * @file nonblocking.c
 * 
 * @brief Implementations of non-blocking transfer routines
 * 
 * @author Daniel Knüppe, Roderick Topütt 
 * 
 * @date May 2019
 */

#include <stdio.h>
#include <semaphore.h>
#include "osmplib.h"
#include "datastructs.h"
#include "nonblocking.h"

int OSMP_CreateRequest(OSMP_Request *request)
{
    request->this = request;
    request->status = async_trans_prepared;
    return OSMP_SUCCESS;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int OSMP_RemoveRequest(OSMP_Request *request) { return OSMP_SUCCESS; }
#pragma GCC diagnostic pop

int OSMP_Wait(OSMP_Request request)
{
    if(pthread_join(request.thread, NULL))
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int OSMP_Test(OSMP_Request request, int *flag)
{
    *flag = request.status;
    return OSMP_SUCCESS;
}

int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest,
               OSMP_Request request)
{
    OSMP_async_arglist arglist = {.send_buf = &buf, 
                                  .recv_buf = NULL,
                                  .count = &count,
                                  .datatype = &datatype,
                                  .dest = &dest,
                                  .source = NULL,
                                  .len = NULL,
                                  .request = &request};

    pthread_create(&(request.this->thread), NULL, &send_wrapper,&arglist);

    return OSMP_SUCCESS;
}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len,
               OSMP_Request request)
{
    OSMP_async_arglist arglist = {.send_buf = NULL, 
                                  .recv_buf = &buf,
                                  .count = &count,
                                  .datatype = &datatype,
                                  .dest = NULL,
                                  .source = &source,
                                  .len = &len,
                                  .request = &request};

    pthread_create(&(request.this->thread), NULL, &recv_wrapper, &arglist);

    return OSMP_SUCCESS;
}

#define args *((OSMP_async_arglist *)arglist)

void *send_wrapper(void *arglist)
{
    (args->request).this->status = async_trans_incomplete;
    if(OSMP_Send(args->send_buf, args->count, args->datatype, args->dest) == OSMP_ERROR)
        pthread_exit(NULL);
    (args->request).this->status = async_trans_complete;

    pthread_exit(&(args->request).this->status);
}

void *recv_wrapper(void *arglist)
{
    (args->request).this->status = async_trans_incomplete;
    if(OSMP_Recv(args->recv_buf, args->count, args->datatype, args->source, args->len) == OSMP_ERROR)
        pthread_exit(NULL);
    (args->request).this->status = async_trans_complete;

    pthread_exit(&(args->request).this->status);
}

#undef args