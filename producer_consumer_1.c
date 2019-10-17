#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>
#define BUFFER_SIZE 24

void *produce(void *i);
void *consume(void *i);

sem_t mutex ; // for synchronization between producer and consumer
sem_t full,empty ; // keeping track of items

int items[BUFFER_SIZE];
int main()
{
    sem_init(&mutex,0,1);
    sem_init(&full,0,0);
    sem_init(&empty,0,BUFFER_SIZE);
    pthread_t producer,consumer;
    int x=0;
    pthread_create(&producer,NULL,produce,&x);
    pthread_create(&consumer,NULL,consume,&x);
    pthread_join(producer,NULL);
    pthread_join(consumer,NULL);
    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    return 0;
}

void * produce(void * i)
{
    int *num=i;
    int index=*num;
    while(1)
    {
        sem_wait(&empty);
        sem_wait(&mutex);
        items[index]=index;
        printf("producing items no %d",index+1);
        sem_post(&mutex);
        sem_post(&full);
        index=(index+1)%BUFFER_SIZE;
    }
}

void* consume(void *i)
{
    int *num=i;
    int index=*num;
    while(1)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        printf("consuming item no %d",items[index]);
        sem_post(&mutex);
        sem_post(&empty);
        index=(index+1)%BUFFER_SIZE;
    }
}