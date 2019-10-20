#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int empty,full,lock;

int BUFFER_SIZE=5,P=10;
int MAX_DELAY=5000;

struct sembuf up ={0,1,0},down={0,-1,0};

struct buffer
{
	int queue[10],out,in,cnt;
};

int producer(int n,struct buffer *buf)
{
	srand(n);
	for(int i=0;i<n;i++)
	{
		int sleep_time=rand()% MAX_DELAY;
		int item=rand()%100;
		sleep(1);
		semop(empty,&down,1);
		semop(lock,&down,1);
		buf->queue[buf->in]=item;
		buf->in=(buf->in+1)%BUFFER_SIZE;
		printf("item %d is produced \n",item);
		semop(lock,&up,1);
		semop(full,&up,1);
	}
	return 0;
}

int consumer(int n, struct buffer *buf)
{
	srand(n);
	for(int i=0;i<n;i++)
	{
		int sleep_time=rand()%MAX_DELAY;
		int item=rand()%100;
		sleep(1);
		semop(full,&down,1);
		semop(lock,&down,1);
		printf("items %d is consumed \n",buf->queue[buf->out]);
		buf->out=(buf->out+1)%BUFFER_SIZE;
		semop(empty,&up,1);
		semop(lock,&up,1);
	}
	return 0;
}

int main()
{
	int i,status;
	pid_t proc,cons;
	struct buffer *buf;
	int shmid=shmget(IPC_PRIVATE,1,IPC_CREAT | 0600);
	
	empty = semget(IPC_PRIVATE,1,IPC_CREAT | 0600);
	full = semget(IPC_PRIVATE,1,IPC_CREAT | 0600);
	lock = semget(IPC_PRIVATE,1,IPC_CREAT | 0600);
	
	buf=shmat(shmid,NULL,0);
	buf->in=0;
	buf->out=0;
	semctl(empty,0,SETVAL,BUFFER_SIZE);
	semctl(full,0,SETVAL,0);
	semctl(lock,0,SETVAL,1);
	

	int pid=fork();
	if(pid==0)
	{
		int ret = consumer(P,buf);
		exit(0);
	}
	else
	{
		producer(P,buf);
		cons=pid;
	}
	printf("waiting for consumers \n");
	waitpid(cons,&status,0);
	shmdt(buf);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(full,0,IPC_RMID,0);
	semctl(empty,0,IPC_RMID,0);
	semctl(lock,0,IPC_RMID,0);
	return 0;
}
