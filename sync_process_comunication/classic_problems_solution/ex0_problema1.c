#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t s1, s2, s3, s4;

void *task0(void *arg)
{
	int i;

	for (i = 1; i <= 200; i++)
		printf("%d ", i);
	sem_post(&s1);
}

void *task1(void *arg)
{
	int i;

	sem_wait(&s1);
	for (i = 201; i <= 400; i++)
		printf("%d ", i);
	sem_post(&s2);
}

void *task2(void *arg)
{
	int i;

	sem_wait(&s2);
	for (i = 401; i <= 600; i++)
		printf("%d ", i);
	sem_post(&s3);
}

void *task3(void *arg)
{
	int i;

	sem_wait(&s3);
	for (i = 601; i <= 800; i++)
		printf("%d ", i);
	sem_post(&s4);
}

void *task4(void *arg)
{
	int i;

	sem_wait(&s4);
	for (i = 801; i <= 1000; i++)
		printf("%d ", i);
}

int main(void)
{
	long int i;
	pthread_t t0, t1, t2, t3, t4;

	sem_init(&s1, 0, 0);
	sem_init(&s2, 0, 0);
	sem_init(&s3, 0, 0);
	sem_init(&s4, 0, 0);

	pthread_create(&t0, NULL, task0, NULL);
	pthread_create(&t1, NULL, task1, NULL);
	pthread_create(&t2, NULL, task2, NULL);
	pthread_create(&t3, NULL, task3, NULL);
	pthread_create(&t4, NULL, task4, NULL);

	pthread_exit(NULL);
	
	return(0);
}
