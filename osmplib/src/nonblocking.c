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
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    request = malloc(sizeof(OSMP_req));
    if(request == NULL)
        return OSMP_ERROR;
    ((OSMP_req *)request)->status = async_trans_prepared;
    return OSMP_SUCCESS;
}

int OSMP_RemoveRequest(OSMP_Request *request) 
{
    free(request);
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    return OSMP_SUCCESS; 
}

int OSMP_Wait(OSMP_Request request)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    if(pthread_join(((OSMP_req *)request)->thread, NULL))
        return OSMP_ERROR;
    return OSMP_SUCCESS;
}

int OSMP_Test(OSMP_Request request, int *flag)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    *flag = ((OSMP_req *)request)->status;
    return OSMP_SUCCESS;
}

int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest,
               OSMP_Request request)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    ((OSMP_req *)request)->args.send_buf = buf;
    ((OSMP_req *)request)->args.count    = count;
    ((OSMP_req *)request)->args.datatype = datatype;
    ((OSMP_req *)request)->args.dest     = dest;
    ((OSMP_req *)request)->args.request  = request;

    int ret = pthread_create(&(((OSMP_req *)request)->thread), 
                             NULL, 
                             &send_wrapper, 
                             &(((OSMP_req *)request)->args));

    return ret;
}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len,
               OSMP_Request request)
{
    if(osmp_globals.shm_base == NULL)
        return OSMP_ERROR;
    ((OSMP_req *)request)->args.recv_buf = buf;
    ((OSMP_req *)request)->args.count    = count;
    ((OSMP_req *)request)->args.datatype = datatype;
    ((OSMP_req *)request)->args.source   = source;
    ((OSMP_req *)request)->args.len      = len;
    ((OSMP_req *)request)->args.request  = request;

    int ret = pthread_create(&(((OSMP_req *)request)->thread), 
                             NULL,
                             &recv_wrapper, 
                             &(((OSMP_req *)request)->args));

    return ret;
}

#define args ((OSMP_async_arglist *)arglist)

void *send_wrapper(void *arglist)
{
    ((OSMP_req *)args->request)->status = async_trans_incomplete;
    if(OSMP_Send(args->send_buf, 
                 args->count, 
                 args->datatype, 
                 args->dest))
        pthread_exit(&(osmp_globals.thread_error));
    ((OSMP_req *)args->request)->status = async_trans_complete;

    pthread_exit(&((OSMP_req *)args->request)->status);
}

void *recv_wrapper(void *arglist)
{
    ((OSMP_req *)args->request)->status = async_trans_incomplete;
    if(OSMP_Recv(args->recv_buf,
                 args->count,
                 args->datatype,
                 args->source,
                 args->len))
        pthread_exit(&(osmp_globals.thread_error));
    ((OSMP_req *)args->request)->status = async_trans_complete;

    pthread_exit(&((OSMP_req *)args->request)->status);
}

#undef args