#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

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
}

void* philosopher(void* philosopher_number){
    while(1){
        int* i = philosopher_number;
        sleep(1);
        pickup_forks(*i);
        sleep(0);
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
