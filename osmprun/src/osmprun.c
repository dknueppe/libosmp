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
    int shm_fd;
    
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

    // create truncate and map the shared memory into the library manager
    void *shm;
    char shm_name[18];
    size_t shm_size;
    get_shm_name18(shm_name);
    shm_size = (sizeof(char) * OSMP_MAX_SLOTS);
    shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(shm_fd == -1) {
        printf("Error calling shm_open:\n%s",strerror(errno));
        exit(1);
    }
    if(ftruncate(shm_fd, shm_size) == -1) {
        printf("Error calling ftruncate:\n%s",strerror(errno));
        exit(1);
    }
    shm = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm == MAP_FAILED) {
        printf("Error calling mmap:\n%s",strerror(errno));
        exit(1);
    }

    // allocate and initialize dummy message
    char *messages = malloc(shm_size);
    if(messages == NULL)
        exit(1);
    int A = 0;
    for(unsigned int i = 0; i < sizeof(int); i++)
        A += ('A' << (8*i));
    memset(messages, A, shm_size);
    *(size_t *)messages = shm_size;
    messages[shm_size - 1] = '\0';

    // initialize shared memory from dummy message
    memcpy(shm, messages, shm_size);

    free(messages);

    // launch num_proc child processes 
    pid_t pid_list[num_proc];
    int ret_exec;
    argv[0] = shm_name;
    for(int i = 0; i < num_proc; i++) {
        pid_list[i] = fork();
        if(pid_list[i] == -1) {
            exit(1);
        } else if (pid_list[i] == 0) {
            ret_exec = execvp(program, argv);
            if (ret_exec == -1)
                exit(1);
        }
    }

    // wait for child processes
    for(int i = 0; i < num_proc; i++) {
        int status;
        pid_t tmp = wait(&status);
        printf("child with pid [%d] exited\n", tmp);
    }

    if(munmap(shm, shm_size) == -1) {
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
    srand(++osmp_shm_name_seed + time(NULL));
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
