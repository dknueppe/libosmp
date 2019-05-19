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
    OSMP_msg_node* node = pop(&base->empty_list);
    node->datatype = datatype;
    node->len = count * OSMP_sizeof(datatype);
    node->receiver = dest;
    node->sender = rank;
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
    memcpy(buf, node->msg_buf, count * OSMP_sizeof(datatype));
    int ret;
    if(datatype != node->datatype)
        ret = node->datatype;
    else
        ret = OSMP_SUCCESS;
    push(node, &base->empty_list);
    return ret;
}
