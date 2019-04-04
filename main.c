#include <stdio.h>
#include <unistd.h>

int main()
{
    int status;
    printf("OSMPstarter V0.1 running\n");
    printf("starting processes\n");
    status = fork();
    if (status == 0){
        printf("I am the child process\n");
        execl("echo", "This", "is", "the", "child", NULL);
    } else {
        printf("Child PID = %d\n", status);
    }
    return 0;
}