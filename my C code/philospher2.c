#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

sem_t room;
sem_t chopstick[5];

void * philosopher(void *);
void eat(int phil, int c2);
int main()
{
	int i,a[5];
	pthread_t tid[5];
	
	sem_init(&room,0,4); //in sem_init() 0 means this thread share resource between threads else it share resource between processes
	
	for(i=0;i<5;i++)
		sem_init(&chopstick[i],0,1);
		
	for(i=0;i<5;i++){
		a[i]=2*i%5; //Set the phelosopher that entering room to have 1 seat distance
		pthread_create(&tid[i],NULL,philosopher,(void *)&a[i]);
	}
	for(i=0;i<5;i++)
		pthread_join(tid[i],NULL);
}

void * philosopher(void * num)
{
	int phil=*(int *)num;

	sem_wait(&room);
	printf("\nPhilosopher %d has entered room",phil);
	sem_wait(&chopstick[phil]);
	sem_wait(&chopstick[(phil+1)%5]);

	//sleep(1);
	eat(phil, (phil+1)%5);
	sleep(2);
	printf("\nPhilosopher %d has finished eating",phil);

	sem_post(&chopstick[(phil+1)%5]);
	sem_post(&chopstick[phil]);
	sem_post(&room);
}

void eat(int phil, int c2)
{
	printf("\nPhilosopher %d is eating spagetti using chopsticks %d and %d", phil, phil, c2);
}
/* BY - ANUSHKA DESHPANDE */
//this code has been modified