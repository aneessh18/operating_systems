#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int lock,write_block;
int MAX_DELAY=5000;
struct memory
{
    int read_count;
};

struct sembuf up={0,1,0},down={0,-1,0};

void reader(int n,struct memory *buf)
{
    srand(n);
    for(int i=0;i<n;i++)
    {
        int sleep_time=rand()%MAX_DELAY;
        int item=rand()%100;
        sleep(1);
        semop(lock,&down,1);
        buf->read_count+=1;
        if(buf->read_count==1)
        {
            semop(write_block,&up,1);
        }
        semop(lock,&up,1);
        printf("reader completed reading %d data\n",item);
        semop(lock,&down,1);
        buf->read_count--;
        if(buf->read_count==0)
        {
            semop(write_block,&down,1);
        }
        semop(lock,&up,1);
    }
}

void writer(int n,struct memory* buf)
{
    srand(n);
    for(int i=0;i<n;i++)
    {
        int sleep_time=rand()%MAX_DELAY;
        int item=rand()%100;
        sleep(1);
        semop(write_block,&down,1);
        printf("writer completed writing %d data\n",item);
        semop(write_block,&up,1);
    }
}

int main()
{
    int i,status;
    pid_t r,w;
    struct memory *buf; 
    int shmid=shmget(IPC_PRIVATE,1,IPC_CREAT | 0600);
    lock=semget(IPC_PRIVATE,1,IPC_CREAT | 0600);
    write_block=semget(IPC_PRIVATE,1,IPC_CREAT | 0600);

    buf=shmat(shmid,NULL,0);
    buf->read_count=0;
    semctl(lock,0,SETVAL,1);
    semctl(write_block,0,SETVAL,1);
    int n=5 ; // no of readers and writers
    int pid=fork();
    if(pid==0) // children 
    {
        writer(n,buf);
        exit(0);
    }
    else
    {
        reader(n,buf);
        r=pid;
    }
    printf("waiting for writers to write \n");
    waitpid(r,&status,0);
    shmdt(buf);
    shmctl(shmid,IPC_RMID,NULL);
    semctl(lock,0,IPC_RMID,0);
    semctl(write_block,0,IPC_RMID,0);
    return 0;
    
}