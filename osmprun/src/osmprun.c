/**
 * @file osmprun.c
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
#include <sys/types.h>
#include <getopt.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "osmplib.h"
#include "osmprun.h"

#define NUM_PROC 3
#define PROG "build/osmpclient"

int main (int argc, char *argv[])
{
    int num_proc = NUM_PROC;
    char *program = PROG;
    
    int opt;
    while((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch(opt) {
        case 'n':
            num_proc = atoi(optarg);
            break;
        case 'p':
            program = optarg;
            break;
        default:
            printf("Unrecognized option!\n");
            exit(EXIT_FAILURE);
        }
    }

    /* create truncate and map the shared memory into the library manager */
    char shm_name[18];
    size_t shm_size;
    get_shm_name18(shm_name);
    shm_size = (sizeof(OSMP_base) + (num_proc * sizeof(OSMP_pcb)));
    g_shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(g_shm_fd == -1) {
        printf("Error calling shm_open:\n%s",strerror(errno));
        exit(1);
    }
    if(ftruncate(g_shm_fd, shm_size) == -1) {
        printf("Error calling ftruncate:\n%s",strerror(errno));
        exit(1);
    }
    g_shm = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, g_shm_fd, 0);
    if(g_shm == MAP_FAILED) {
        printf("Error calling mmap:\n%s",strerror(errno));
        exit(1);
    }

    /* initialize the shared memory to its default values */
    memset(g_shm, -1, shm_size);
    base->shm_size = shm_size;
    base->num_proc = num_proc;
    if(sem_init(&(base->empty_list.max_length), 1, OSMP_MAX_SLOTS))
        exit(1);
    if(sem_init(&(base->empty_list.availabe), 1, 0))
        exit(1);
    if(sem_init(&(base->empty_list.queue_lock), 1, 1))
        exit(1);
    for(int i = 0; i < OSMP_MAX_SLOTS; i++)
        push(&base->messages[i], &base->empty_list);
    for(int i = 0; i < num_proc; i++) {
        if(sem_init(&pcb_list[i].inbox.max_length, 1, OSMP_MAX_SLOTS))
            exit(1);
        if(sem_init(&pcb_list[i].inbox.availabe, 1, 0))
            exit(1);
        if(sem_init(&pcb_list[i].inbox.queue_lock, 1, 1))
            exit(1);
    }

    // launch num_proc child processes 
    int ret_exec;
    pid_t tmp;
    argv[0] = shm_name;
    for(int i = 0; i < num_proc; i++) {
        tmp = fork();
        if(tmp == -1) {
            exit(1);
        } else if (tmp == 0) {
            ret_exec = execvp(program, argv);
            if (ret_exec == -1)
                exit(1);
        }
        pcb_list[i].pid = tmp;
    }

    /* wait for child processes */
    for(int i = 0; i < num_proc; i++) {
        int status;
        pid_t tmp = wait(&status);
        printf("child with pid [%d] exited\n", tmp);
    }

    if(munmap(g_shm, shm_size) == -1) {
        printf("Error calling munmap:\n%s",strerror(errno));
        exit(1);
    }
    if(shm_unlink(shm_name) == -1) {
        printf("Error calling shm_unlink:\n%s",strerror(errno));
        exit(1);
    }

    return 0;
}

void get_shm_name18(char c[])
{
    srand(getpid() + time(NULL));
    c[0] = 'O';
    c[1] = 'S';
    c[2] = 'M';
    c[3] = 'P';
    c[4] = '-';
    c[5] = 'S';
    c[6] = 'H';
    c[7] = 'M';
    c[8] = '-';
    for (int i=9; i<17; i++){
	    c[i] = rand() % 10 + '0';
    }
    c[17] = '\0';
    printf("shared memory name will be: %s\n",c);
}
