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
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("args allocated at: 0x%08X\n", (unsigned int)request->args);
#pragma GCC diagnostic pop
    return OSMP_SUCCESS;
}

int OSMP_RemoveRequest(OSMP_Request *request) 
{
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("args freed at: 0x%08X\n", (unsigned int)request->args);
#pragma GCC diagnostic pop
    free(request->args);
    return OSMP_SUCCESS; 
}

int OSMP_Wait(OSMP_Request request)
{
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("waiting for pthread id: %u\n", (unsigned int)request.thread);
#pragma GCC diagnostic pop
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
    ((OSMP_async_arglist *)((request.self)->args))->send_buf = &buf;
    ((OSMP_async_arglist *)((request.self)->args))->recv_buf = NULL;
    ((OSMP_async_arglist *)((request.self)->args))->count    = &count;
    ((OSMP_async_arglist *)((request.self)->args))->datatype = &datatype;
    ((OSMP_async_arglist *)((request.self)->args))->dest     = &dest;
    ((OSMP_async_arglist *)((request.self)->args))->source   = NULL;
    ((OSMP_async_arglist *)((request.self)->args))->len      = NULL;
    ((OSMP_async_arglist *)((request.self)->args))->request  = request.self;

    int ret = pthread_create(&(request.self->thread), NULL, &send_wrapper, request.self->args);
    printf("pthread_create from Isend returns: %d\n", ret);

    return ret;
}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len,
               OSMP_Request request)
{
    ((OSMP_async_arglist *)((request.self)->args))->send_buf = NULL;
    ((OSMP_async_arglist *)((request.self)->args))->recv_buf = &buf;
    ((OSMP_async_arglist *)((request.self)->args))->count    = &count;
    ((OSMP_async_arglist *)((request.self)->args))->datatype = &datatype;
    ((OSMP_async_arglist *)((request.self)->args))->dest     = NULL;
    ((OSMP_async_arglist *)((request.self)->args))->source   = &source;
    ((OSMP_async_arglist *)((request.self)->args))->len      = &len;
    ((OSMP_async_arglist *)((request.self)->args))->request  = request.self;

    int ret = pthread_create(&(request.self->thread), NULL, &recv_wrapper, request.self->args);
    printf("pthread_create from Irecv returns: %d\n", ret);

    return OSMP_SUCCESS;
}

#define args *((OSMP_async_arglist *)arglist)

void *send_wrapper(void *arglist)
{
    printf("Sendthread says hi\n"); fflush(NULL);
    (args->request).self->status = async_trans_incomplete;
    if(OSMP_Send(args->send_buf, args->count, args->datatype, args->dest) == OSMP_ERROR)
        printf("Blocking send failes in asynchrounus thread!\n");
        //pthread_exit(NULL);
    (args->request).self->status = async_trans_complete;

    pthread_exit(&(args->request).self->status);
}

void *recv_wrapper(void *arglist)
{
    printf("Recvthread says hi\n");fflush(NULL);
    (args->request).self->status = async_trans_incomplete;
    if(OSMP_Recv(args->recv_buf, args->count, args->datatype, args->source, args->len) == OSMP_ERROR)
        pthread_exit(NULL);
    (args->request).self->status = async_trans_complete;

    pthread_exit(&(args->request).self->status);
}

#undef args