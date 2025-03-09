#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define total_ph    5
#define THINKING    2
#define HUNGRY      1
#define EATING      0
#define LEFT        (philosopher_number + 4) % total_ph
#define RIGHT       (philosopher_number + 1) % total_ph

void pickup_forks(int philosopher_number);  
void return_forks(int philosopher_number);
void* philosopher(void* philosopher_number);
void test(int philosopher_number);

int state[total_ph];
int phil[total_ph] = {0, 1, 2, 3, 4};

sem_t mutex;
sem_t S[total_ph];

void test(int philosopher_number)
{
    if (state[philosopher_number] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        state[philosopher_number] = EATING;
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n",
            philosopher_number + 1, LEFT + 1, philosopher_number + 1);

        printf("Philosopher %d is Eating\n", philosopher_number + 1);
        sem_post(&S[philosopher_number]);
    }
}

void pickup_forks(int philosopher_number){
    sem_wait(&mutex);
    state[philosopher_number] = HUNGRY;
    printf("Philosopher %d is Hungry\n", philosopher_number+1);
    test(philosopher_number);
    sem_post(&mutex);
    sem_wait(&S[philosopher_number]);
    sleep(1);

    printf("Philosopher %d has forks %d and %d\n", philosopher_number + 1, LEFT + 1, philosopher_number + 1);

}

void return_forks(int philosopher_number){
    sem_wait(&mutex);  
    state[philosopher_number] = THINKING;
    printf("Philosopher %d putting fork down %d and %d down\n", 
        philosopher_number+1, LEFT+1, philosopher_number+1);
    printf("Philosopher %d is thinking\n", philosopher_number+1);
    test(LEFT);
    test(RIGHT);
    sem_post(&mutex);
    printf("Philosopher %d returned forks %d and %d\n", philosopher_number + 1, LEFT + 1, philosopher_number + 1);

}

void* philosopher(void* philosopher_number){
    int think_time, eat_time;
    clock_t think_start, think_end, eat_start, eat_end;
    while(1){
        int* i = philosopher_number;

        //Time the philosopher takes to think
        think_time = rand() % 3 + 1;
        think_start = clock();
        printf("Philosopher %d is thinking for %d seconds.\n", philosopher_number, think_time);
        sleep(think_time);
        think_end = clock();
        double think_duration = ((double)(think_end - think_start)) / CLOCKS_PER_SEC;
        printf("Philosopher %d thought for %.2f seconds.\n", philosopher_number, think_duration);
        pickup_forks(*i);

        eat_time = rand() % 5 + 1;
        eat_start = clock();
        printf("Philosopher %d is eating for %d seconds.\n", philosopher_number, eat_time);
        sleep(eat_time);
        eat_end = clock();
        double eat_duration = ((double)(eat_end - eat_start)) / CLOCKS_PER_SEC;
        printf("Philosopher %d ate for %.2f seconds.\n", philosopher_number, eat_duration);
        return_forks(*i);
    }
}

int main(){
    int i;
    pthread_t thread_id[total_ph];

    sem_init(&mutex, 0, 1);
    for (i = 0; i < total_ph; i++){
        sem_init(&S[i], 0, 0);
    }

    for (i = 0; i < total_ph; i++){
        pthread_create(&thread_id[i], NULL, philosopher, &phil[i]);
        printf("Philosopher %d is thinking\n", i + 1);
    }

    for (i = 0; i < total_ph; i++){
        pthread_join(thread_id[i], NULL);
    }
   return 0; 
}
