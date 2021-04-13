#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
//#define WITH_SIGNALS

bool wasInterrupt = false;

void terminate(int signal)
{
    printf("Child [%d] Got terminate\n", getpid());
    printf("Child [%d] Closing by termination\n", getpid());
    exit(0);
}

void handler(int signal)
{
    printf("Parent [%d] Got interrupt\n", getpid());
    wasInterrupt = true;
    return;
}

void child_fun()
{
    printf("Child[%d] my father is %d\n", getpid(), getppid());
    sleep(10);
    printf("Child[%d] slept for 10 secs and now exiting...\n", getpid());
    exit(0);
}

int main()
{
    const int NUM_CHILD = 3;
    #ifndef WITH_SIGNALS
    for(int i =0; i<NUM_CHILD; i++)
    {
        printf("Parent[%d] Creating child\n", getpid());
        int child = fork();
        if(child < 0)
        {
            kill(0, SIGTERM);
            waitpid(0, NULL, 0);
            exit(1);
        }
        else if(child == 0)
        {
            child_fun();
        }
        else
        {
            if(i == NUM_CHILD-1)
                break;
            sleep(1);
        }
    }
    printf("Parent[%d] All processes have been created!\n", getpid());

    int status = 0;
    int amount =0;
    while(wait(&status)>=0)
    {
        amount++;
    }
    printf("Parent[%d] All processes have ended, number of exit codes %d\n", getpid(), amount);

    #else

    //ignore all
    signal(SIGABRT, SIG_IGN);
    signal(SIGFPE, SIG_IGN);
    signal(SIGILL, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    //restore SIGCHLD
    signal(SIGCHLD, SIG_DFL);
    //handle interrupt
    signal(SIGINT, handler);
    int procs[NUM_CHILD];

    for(int i =0; i<NUM_CHILD; i++)
    {
        printf("Parent[%d] Creating child\n", getpid());
        int child = fork();
        if(child < 0)
        {
            kill(0, SIGTERM);
            waitpid(0, NULL, 0);
            exit(1);
        }
        else if(child == 0)
        {
            signal(SIGINT, SIG_IGN);
            signal(SIGTERM, terminate);
            child_fun();
        }
        else
        {
            procs[i] = child;

            if(i == NUM_CHILD-1)
                break;

            sleep(1);
            if(wasInterrupt)
            {
                printf("Parent[%d] Got mark\n", getpid());
                for(int j =0; j<=i; j++)
                {
                    printf("Parent[%d] Sending terminate to child %d\n", getpid(), procs[j]);
                    kill(procs[j], SIGTERM);
                }
                break;
            }
        }
    }
    if(!wasInterrupt)
        printf("Parent[%d] All processes have been created!\n", getpid());

    int status = 0;
    int amount =0;
    while(wait(&status)>=0)
    {
        amount++;
    }
    printf("Parent[%d] All processes have ended, number of exit codes %d\n", getpid(), amount);
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    #endif
    return 0;
}
