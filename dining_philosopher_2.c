#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int N=5 ; //no of philosophers and chopsticks

int chopsticks[5];
int MAX_DELAY=5000;
struct sembuf up={0,1,0},down={0,-1,0};

void dining(int n)
{
    srand(n);
    int sleep_time=rand()%MAX_DELAY;
    sleep(1);
    printf("philosopher %d is hungry \n",n);
    int left=(n+4)%N,right=(n+1)%N;
    semop(chopsticks[left],&down,1);
    semop(chopsticks[right],&down,1);
    printf("philosopher %d is eating \n",n);
    //sleep(1);
    semop(chopsticks[left],&up,1);
    semop(chopsticks[right],&up,1);
    printf("philosopher %d is thinking \n",n);
}

int main()
{
    int i;
    for(int j=0;j<N;j++)
    chopsticks[j]=semget(IPC_PRIVATE,1,IPC_CREAT | 0600);
    for(int j=0;j<N;j++)
    semctl(chopsticks[j],0,SETVAL,1);

    for(i=0;i<N;i++)
    {
        int pid=fork();
        if(pid==0)
        {
            dining(i);
            break;
        }       
    }
    wait(NULL);
    for(int j=0;j<N;j++)
    {
        semctl(chopsticks[j],0,IPC_RMID,0);
    }
    return 0;
}
