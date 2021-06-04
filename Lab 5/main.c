#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

const int PHILOSOPHERS_NUM = 5; //number of philosophers
const int MEALS_NUM = 1; //number of iterations before philosophers finish
const int SLEEP_TIME = 1;
//semaphore ids
int forks;
int not_at_table; //start dinner when semaphore reaches 0

int philosopher(int n);
void grab_forks(int left_fork_id);
void put_away_forks(int left_fork_id);

int main(){
    if(PHILOSOPHERS_NUM < 2)
    {
        printf("Not enough philosophers\n");
        return 0;
    }

    int status;

    int philosophers[PHILOSOPHERS_NUM];

    //setup semaphores with read/write
    forks = semget(IPC_PRIVATE, PHILOSOPHERS_NUM, IPC_CREAT | 0600);
    for(int i=0; i<PHILOSOPHERS_NUM; i++)
    {
        semctl(forks, i, SETVAL, 1);
    }
    not_at_table = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);

    // set value to number of philosophers
    semctl(not_at_table, 0, SETVAL, PHILOSOPHERS_NUM);

    //create philosophers
    for(int i=0; i < PHILOSOPHERS_NUM; i++)
    {
        int pid = fork();
        if(pid == 0){
            int ret = philosopher(i);
            exit(1);
        }
        else{
            philosophers[i] = pid;
        }
    }

    // Wait for everyone to eat
    while(wait(&status)>0)
    {

    }

    // Clear up
    semctl(forks, 0, IPC_RMID, 0);
    semctl(not_at_table, 0, IPC_RMID, 0);
    return 0;
}

// Code for dining philosopher child processes
int philosopher(int n){
    int i, j, first, second;
    struct sembuf op;
    op.sem_flg = 0;

    //There is no fork PHILOSOPHERS_NUM
    int left_fork = (n < PHILOSOPHERS_NUM-1)? n+1 :0;
    //not_at_table--
    op.sem_op = -1;
    op.sem_num = 0;
    semop(not_at_table, &op, 1);
    printf("Philosopher %d sits down\n", n);

    //Wait until all processes decrease not_at_table to 0
    op.sem_op = 0;
    op.sem_num = 0;
    semop(not_at_table, &op, 1);

    //Main loop of thinking/eating cycles
    for(i = 0; i < MEALS_NUM; i++)
    {
        printf("Philosopher %d is thinking\n",n);
        sleep(SLEEP_TIME);
        grab_forks(left_fork);

        printf("Philosopher %d is eating\n",n);
        sleep(SLEEP_TIME);
        put_away_forks(left_fork);
    }

    printf("Philosopher %d is done\n",n);
    exit(n);
}

void grab_forks(int left_fork_id)
{
    int first, philosopher;
    struct sembuf op;
    op.sem_flg = 0;

    if(left_fork_id == 0)
    {
        first=left_fork_id;
        left_fork_id = PHILOSOPHERS_NUM-1;
        philosopher = PHILOSOPHERS_NUM-1;
    }
    else
    {
        first=left_fork_id-1;
        philosopher = left_fork_id-1;
    }
    printf("Philosopher %d tries to take fork %d\n", philosopher, first);
    //get first fork
    op.sem_op = -1;
    op.sem_num = first;
    semop(forks, &op, 1);
    printf("Philosopher %d took fork %d\n", philosopher, first);
    printf("Philosopher %d tries to take fork %d\n", philosopher, left_fork_id);
    //get second fork
    op.sem_op = -1;
    op.sem_num = left_fork_id;
    semop(forks, &op, 1);
    printf("Philosopher %d took fork %d\n", philosopher, left_fork_id);
}

void put_away_forks(int left_fork_id)
{
    int first;
    int philosopher = left_fork_id == 0? PHILOSOPHERS_NUM-1 : left_fork_id-1;
    struct sembuf op;
    op.sem_flg = 0;

    if(left_fork_id == 0)
    {
        first=left_fork_id;
        left_fork_id = PHILOSOPHERS_NUM-1;
        philosopher = PHILOSOPHERS_NUM-1;
    }
    else
    {
        first=left_fork_id-1;
        philosopher = left_fork_id-1;
    }
    //release first fork
    op.sem_op = 1;
    op.sem_num = first;
    semop(forks, &op, 1);
    printf("Philosopher %d puts down fork %d\n", philosopher, first);
    //release second fork
    op.sem_op = 1;
    op.sem_num = left_fork_id;
    semop(forks, &op, 1);
    printf("Philosopher %d puts down fork %d\n", philosopher, left_fork_id);
}


