/**
 * @file blocking.c
 * 
 * @brief The osmplib header 
 * 
 * @author Daniel Knüppe, Roderick Topütt
 * 
 * @date April 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "queue.h"
#include "osmplib.h"

int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest)
{
    int rank;
    if(buf == NULL)
        return OSMP_ERROR;
    if(OSMP_MAX_PAYLOAD_LENGTH < count)
        return OSMP_ERROR;
    if(datatype < OSMP_UNDEFINED || datatype > OSMP_UNDEFINED_UPPER)
        return OSMP_ERROR;
    if(OSMP_Rank(&rank) == OSMP_ERROR)
        return OSMP_ERROR;
    if((unsigned)dest >= base->num_proc)
        return OSMP_ERROR;
    if(count * OSMP_sizeof(datatype) > OSMP_MAX_PAYLOAD_LENGTH)
        return OSMP_ERROR;
    OSMP_msg_node* node = pop(&base->empty_list);
    node->datatype = datatype;
    node->len = count * OSMP_sizeof(datatype);
    node->receiver = dest;
    node->sender = rank;
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("SEND: copying from 0x%08X to 0x%08X\n", (unsigned int)buf, (unsigned int)node->msg_buf); fflush(NULL);
#pragma GCC diagnostic pop
    memcpy(node->msg_buf, buf, count);
    push(node, &pcb_list[dest].inbox);
    return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len)
{
    int rank;
    if(OSMP_Rank(&rank) == OSMP_ERROR)
        return OSMP_ERROR;
    OSMP_msg_node *node = pop(&pcb_list[rank].inbox);
    *source = node->sender;
    *len = node->len;

    size_t size = node->len > count * OSMP_sizeof(datatype) ?
                  node->len : count * OSMP_sizeof(datatype);
    if(size > OSMP_MAX_PAYLOAD_LENGTH){
        push(node, &base->empty_list);
        return OSMP_ERROR;
    }
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("RECV: copying from 0x%08X to 0x%08X\n", (unsigned int)node->msg_buf, (unsigned int)buf); fflush(NULL);
#pragma GCC diagnostic pop
    memcpy(buf, node->msg_buf, size);
    push(node, &base->empty_list);

    int ret;
    if(datatype != node->datatype)
        ret = node->datatype;
    else
        ret = OSMP_SUCCESS;
    return ret;
}
