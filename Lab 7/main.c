#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define LEFT (i + N- 1) % N
#define RIGHT ( i + 1 ) % N

#define THINKING 1
#define HUNGRY 2
#define EATING 3
#define up pthread_mutex_unlock
#define down pthread_mutex_lock

const int MEALS_NUM = 3;
const int SLEEP_TIME = 1;

pthread_mutex_t m;
int state[N];
pthread_mutex_t s[N];

pthread_t philosophers[N];

void *philosopher (void *num);
void grab_forks (int);
void test (int);
void put_away_forks (int);

int main (int argc, char **argv)
{
	int i;

	pthread_mutex_init(&m, NULL);

	//Create a thread for each philosopher
	for (i = 0; i < N; i++){
		//initialize each thread by calling the routine philosopher()
		state[i] = THINKING;
		pthread_mutex_init(&s[i], NULL);
		down(&s[i]);

		if(pthread_create(&philosophers[i], NULL, philosopher, (void*) i)!=0)
		{
            printf("Could not create process\n");
            return(1);
		}
	}

	//Wait for the threads to exit
	for (i = 0; i < N; i++){
		pthread_join (philosophers[i], NULL);
	}

    pthread_mutex_destroy(&m);
    for (i = 0; i < N; i++)
    {
        pthread_mutex_destroy(&s[i]);
    }

	pthread_exit(NULL);

	return 0;
}

void *philosopher (void *num){
	int meals = MEALS_NUM;
	int current_phil = (int)num;

	printf("Philosopher %d sat down at the table\n", current_phil);
	while(meals>0)
	{
        printf("Philosopher %d is thinking\n", current_phil);
        sleep(SLEEP_TIME);
        grab_forks(current_phil);
        printf("Philosopher %d is eating\n", current_phil);
        meals--;
        sleep(SLEEP_TIME);
        put_away_forks(current_phil);
	}
}

void grab_forks(int i)
{
    down(&m);
    printf("Philosopher %d is hungry\n", i);
    state[i] = HUNGRY;
    test(i);
    up(&m);
    down(&s[i]);
}

void put_away_forks(int i)
{
    down(&m);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    up(&m);
}

void test(int i)
{
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        printf("Philosopher %d took forks\n", i);
        up(&s[i]);
    }
}


