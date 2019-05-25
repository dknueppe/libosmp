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
#include <stdlib.h>
#include <semaphore.h>
#include "osmplib.h"
#include "datastructs.h"
#include "nonblocking.h"

int OSMP_CreateRequest(OSMP_Request *request)
{
    request->self = request;
    request->status = async_trans_prepared;
    request->args = malloc(sizeof(OSMP_async_arglist));
    if(request->args == NULL)
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int OSMP_RemoveRequest(OSMP_Request *request) 
{
    free(request->args);
    return OSMP_SUCCESS; 
}

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
    ((OSMP_async_arglist *)((request.self)->args))->send_buf = buf;
    ((OSMP_async_arglist *)((request.self)->args))->count    = count;
    ((OSMP_async_arglist *)((request.self)->args))->datatype = datatype;
    ((OSMP_async_arglist *)((request.self)->args))->dest     = dest;
    ((OSMP_async_arglist *)((request.self)->args))->request  = request;

    int ret = pthread_create(&(request.self->thread), 
                             NULL, 
                             &send_wrapper, 
                             request.self->args);

    return ret;
}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len,
               OSMP_Request request)
{
    ((OSMP_async_arglist *)((request.self)->args))->recv_buf = buf;
    ((OSMP_async_arglist *)((request.self)->args))->count    = count;
    ((OSMP_async_arglist *)((request.self)->args))->datatype = datatype;
    ((OSMP_async_arglist *)((request.self)->args))->source   = source;
    ((OSMP_async_arglist *)((request.self)->args))->len      = len;
    ((OSMP_async_arglist *)((request.self)->args))->request  = request;

    int ret = pthread_create(&(request.self->thread), 
                             NULL,
                             &recv_wrapper, 
                             request.self->args);

    return ret;
}

#define args ((OSMP_async_arglist *)arglist)

void *send_wrapper(void *arglist)
{
    ((args->request).self)->status = async_trans_incomplete;
    if(OSMP_Send(args->send_buf, 
                 args->count, 
                 args->datatype, 
                 args->dest))
        pthread_exit(&(osmp_globals.thread_error));
    ((args->request).self)->status = async_trans_complete;

    pthread_exit(&(((args->request).self)->status));
}

void *recv_wrapper(void *arglist)
{
    ((args->request).self)->status = async_trans_incomplete;
    if(OSMP_Recv(args->recv_buf,
                 args->count,
                 args->datatype,
                 args->source,
                 args->len))
        pthread_exit(&(osmp_globals.thread_error));
    ((args->request).self)->status = async_trans_complete;

    pthread_exit(&(((args->request).self)->status));
}

#undef args