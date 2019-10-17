#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define N 5  // the number of philosphers 
pthread_mutex_t chopsticks[N];  // they are shared between the philosophers
pthread_t philosophers[N] ; 
int philosophers_id[N];
int sleeptime=3;
void * share_the_chopsticks(void *id); //driving function for pthreads
int main()
{
    for(int i=0;i<N;i++)
    philosophers_id[i]=i;
    for(int i=0;i<N;i++)
    pthread_mutex_init(&chopsticks[i],NULL);
    for(int i=0;i<N;i++)
    pthread_create(&philosophers[i],NULL,share_the_chopsticks,&philosophers_id[i]);
    for(int i=0;i<N;i++)
    pthread_join(philosophers[i],NULL);
    return 0;
}

void * share_the_chopsticks(void *id)
{
    int* num= (int* ) id;
    int left=(*num+4)%N,right=(*num+1)%N;
    printf("philosopher %d is hungry \n",*num);  
    pthread_mutex_lock(&chopsticks[left]);
    pthread_mutex_lock(&chopsticks[right]);  // locking both left and right chopsticks 
    printf("philosopher %d is eating \n",*num);
    sleep(3);
    pthread_mutex_unlock(&chopsticks[left]);
    pthread_mutex_unlock(&chopsticks[right]);
    printf("philosopher %d is thinking \n",*num);
}
