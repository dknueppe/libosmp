/**
 * @file osmprun.c
 * 
 * @brief The osmp runtime manager 
 * 
 * @author Daniel Knüppe
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
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "osmplib.h"
#include "osmprun.h"
#include "queue.h"

#define VERSION "0.5.1"

#define NUM_PROC 1
#define PROG "build/osmpclient"

int main (int argc, char *argv[], char *envp[])
{
    int num_proc = NUM_PROC;
    char *program = PROG;
    
    /* parsing input options */
    int opt;
    static const struct option opt_long[] = {
        {"child",   no_argument, NULL, 'c'},
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'V'}
        };
    int index_child_argv = argc -1;
    int option_index;
    while((opt = getopt_long(argc, argv, "Vhn:p:c", opt_long, &option_index)) != -1) {
        switch(opt) {
        case 'h':
            print_help();
            break;
        case 'V':
            print_version();
            break;
        case 'n':
            num_proc = atoi(optarg);
            break;
        case 'p':
            program = optarg;
            break;
        case 'c':
            index_child_argv = optind -1;
            optind = argc;
            break;
        default:
            print_help();
            break;
        }
    }

    /* creating the name for the shared memory and the string for 
     * the env list used in the later called execve
     */
    char shm_name[18];
    get_shm_name18(shm_name);
    char *shm_name_env = malloc(28);
    sprintf(shm_name_env, "OSMPSHM=%s", shm_name);

    /* create truncate and map the shared memory into the library manager */
    size_t shm_size;
    shm_size = (sizeof(OSMP_base) + (num_proc * sizeof(OSMP_pcb)));
    osmp_globals.shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0644);
    if(osmp_globals.shm_fd == -1) {
        printf("Error calling shm_open:\n%s",strerror(errno));
        exit(1);
    }
    if(ftruncate(osmp_globals.shm_fd, shm_size) == -1) {
        printf("Error calling ftruncate:\n%s",strerror(errno));
        exit(1);
    }
    osmp_globals.shm_base = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, osmp_globals.shm_fd, 0);
    if(osmp_globals.shm_base == MAP_FAILED) {
        printf("Error calling mmap:\n%s",strerror(errno));
        exit(1);
    }

    /* initialize the shared memory to its default values */
    memset(osmp_globals.shm_base, -1, shm_size);
    base->shm_size = shm_size;
    base->num_proc = num_proc;
    /* initialize the semaphores in the empty list and assign messages */
    if(init_queue(&base->empty_list, OSMP_MAX_SLOTS))
        exit(1);
    for(int i = 0; i < OSMP_MAX_SLOTS; i++)
        push(&base->messages[i], &base->empty_list);
    /* initialize the semaphores in the process control blocks */
    for(int i = 0; i < num_proc; i++) {
        if(init_queue(&pcb_list[i].inbox, OSMP_MAX_MESSAGES_PROC))
            exit(1);
    }

    /* prepare child_argv from argv for use in the child execve */
    int size =  argc - index_child_argv;
    char **child_argv = malloc((size + 1) * sizeof(*child_argv));
    for(int i = 0; i < size; i++)
        child_argv[i] = argv[i + index_child_argv];
    child_argv[size] = NULL;
    child_argv[0] = program;
    /* prepare child_envp from envp for use in the child execve */
    size = 0;
    while(envp[size] != NULL)
        size++;
    char **child_envp = malloc((size + 2) * sizeof(*child_envp));
    for(int i = 0; i <= size; i++)
        child_envp[i] = envp[i];
    child_envp[size+1] = NULL;
    child_envp[size] = shm_name_env;

    /* launch num_proc child processes */
    int ret_exec;
    argv[0] = shm_name;
    for(int i = 0; i < num_proc; i++) {
        pid_t tmp = fork();
        if(tmp == -1) {
            exit(1);
        } else if (tmp == 0) {
            ret_exec = execve(program, child_argv, child_envp);
            if (ret_exec == -1)
                exit(1);
        }
        pcb_list[i].pid = tmp;
    }

    /* wait for child processes to exit */
    for(int i = 0; i < num_proc; i++) {
        int status;
        pid_t tmp = wait(&status);
        /*printf("child with pid [%d] exited\n", tmp);*/
    }

    /* de initialize the queues */
    destroy_queue(&base->empty_list);
    for(int i = 0; i < num_proc; i++)
        destroy_queue(&pcb_list[i].inbox);

    /* release the shared memory related resources */
    if(munmap(osmp_globals.shm_base, shm_size) == -1) {
        printf("Error calling munmap:\n%s",strerror(errno));
        exit(1);
    }
    if(shm_unlink(shm_name) == -1) {
        printf("Error calling shm_unlink:\n%s",strerror(errno));
        exit(1);
    }

    free(shm_name_env);
    free(child_argv);
    free(child_envp);
    
    return 0;
}

void print_help()
{
    char help_txt[] = 
#include "help.txt"
    printf(help_txt);
    exit(0);
}

void print_version()
{
    char version_txt[] =
#include "version.txt"
    printf(version_txt, VERSION);
    exit(0);
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
    //printf("shared memory name will be: %s\n",c);
}
