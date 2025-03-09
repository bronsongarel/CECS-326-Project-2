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

int state[total_ph];                                                       //state = {'HUNGRY', 'EATING', 'THINKING'}
int phil[total_ph] = {0, 1, 2, 3, 4};                                      //5 total philosophers

sem_t mutex;
sem_t S[total_ph];

void test(int philosopher_number)
{
    if (state[philosopher_number] == HUNGRY                                 //check if the current philosopher is hungry and the 
        && state[LEFT] != EATING                                            //forks to the left and right are not in use
        && state[RIGHT] != EATING) {
        state[philosopher_number] = EATING;                                 //change state to eating
        sleep(2);                                                           //sleep to perform action
        printf("Philosopher %d takes fork %d and %d\n",                     //take forks based on eating state
            philosopher_number + 1, LEFT + 1, philosopher_number + 1);      //and display to the terminal

        printf("Philosopher %d is Eating\n", philosopher_number + 1);       //Since the philosopher has forks, display to the terminal that they are eating
        sem_post(&S[philosopher_number]);                                   //release the semaphore to allow other philosophers to act
    }
}

void pickup_forks(int philosopher_number){
    sem_wait(&mutex);                                               //'locks' semaphore to allow changes to be made only to this philosopher
    state[philosopher_number] = HUNGRY;                             //pickup forks changes state to hungryto convey it want to pick a fork up
    printf("Philosopher %d is Hungry\n", philosopher_number+1);     //display to terminal our new state
    test(philosopher_number);                                       //run test on current philosopher number
    sem_post(&mutex);                                               //release the semaphore
    sem_wait(&S[philosopher_number]);                               //lock the resources to the philosopher they were give to
    sleep(1);                                                       //sleep to perform action

    printf("Philosopher %d has forks %d and %d\n", philosopher_number + 1, LEFT + 1, philosopher_number + 1);   //Display to terminal which forks the philosopher picked up

}

void return_forks(int philosopher_number){
    sem_wait(&mutex);                                             //'locks' semaphore to allow changes to be made only to this philosopher
    state[philosopher_number] = THINKING;                         //return forks changes state to thinking in order to make decision about left and right
    printf("Philosopher %d putting fork down %d and %d down\n", 
        philosopher_number+1, LEFT+1, philosopher_number+1);
    printf("Philosopher %d is thinking\n", philosopher_number+1); //displays to terminal philosopher # is thinking at the moment
    test(LEFT);                                                   //test for left fork
    test(RIGHT);                                                  //test for right fork
    sem_post(&mutex);                                             //'unlokcs' the mutex
    printf("Philosopher %d returned forks %d and %d\n", philosopher_number + 1, LEFT + 1, philosopher_number + 1); //displays to terminal which forks the philosopher is putting dowm

}

void* philosopher(void* philosopher_number){
    //variables used to capture different time measurements for the philosophers actions
    int think_time, eat_time;
    clock_t think_start, think_end, eat_start, eat_end;
    while(1){
        int* i = philosopher_number; //captures the current philosopher

        //Time the philosopher takes to think
        think_time = rand() % 10 + 1;       //creates a random amount of time the philosopher will eat for
        think_start = clock();              //ends clock to capture full time the philosopher will be thinking for
        printf("Philosopher %d is thinking for %d seconds.\n", *i, think_time); //displays how long the philosopher is thinking to the terminal
        sleep(think_time);                  //sleeps for the randomly generated amount of time
        think_end = clock();                //ends clock to capture full time the philosopher was thinking for
        double think_duration = ((double)(think_end - think_start)) / CLOCKS_PER_SEC;
        printf("Philosopher %d thought for %.2f seconds.\n", *i, think_duration); //displays how long the philosopher thought to the terminal
        pickup_forks(*i);                   //philosopher will see if he can pick up forks to their left and right

        eat_time = rand() % 10 + 1;         //creates a random amount of time the philosopher will eat for
        eat_start = clock();                //starts the clock to be able to capture time
        printf("Philosopher %d is eating for %d seconds.\n", *i, eat_time); //displays how long the philosopher is eating to the terminal
        sleep(eat_time);                    //sleeps for the randomly generated amount of time
        eat_end = clock();                  //ends clock to capture full time the philosopher ate for
        double eat_duration = ((double)(eat_end - eat_start)) / CLOCKS_PER_SEC;
        printf("Philosopher %d ate for %.2f seconds.\n", *i, eat_duration);     //displays how long the philosopher ate to the terminal
        return_forks(*i);                   //philosopher will see if they can return the forks to their left and right
    }
}

int main(){
    int i;
    pthread_t thread_id[total_ph];
    //initialize the semaphores
    sem_init(&mutex, 0, 1);         
    for (i = 0; i < total_ph; i++){
        sem_init(&S[i], 0, 0);
    }
    //Create threads for each philosopher and display thinking
    for (i = 0; i < total_ph; i++){
        pthread_create(&thread_id[i], NULL, philosopher, &phil[i]);
        printf("Philosopher %d is thinking\n", i + 1);
    }

    
    //Join threads 
    for (i = 0; i < total_ph; i++){
        pthread_join(thread_id[i], NULL);
    }
   return 0; 
}
