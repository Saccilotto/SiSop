#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s1, s2, s3;

void *taska(void *arg)
{
	int i;

	printf("task A\n");
	usleep(100000);
	sem_post(&s1);
}

void *taskb(void *arg)
{
	int i;

	printf("task B\n");
	usleep(100000);
	sem_post(&s2);
}

void *taskc(void *arg)
{
	int i;

	sem_wait(&s1);
	sem_wait(&s2);
	printf("task C\n");
	usleep(100000);
	sem_post(&s3);
	sem_post(&s3);
}

void *taskd(void *arg)
{
	int i;

	sem_wait(&s3);
	printf("task D\n");
	usleep(100000);
}

void *taske(void *arg)
{
	int i;

	sem_wait(&s3);
	printf("task E\n");
	usleep(100000);
}


int main(void)
{
	long int i;
	pthread_t t0, t1, t2, t3, t4;

	sem_init(&s1, 0, 0);
	sem_init(&s2, 0, 0);
	sem_init(&s3, 0, 0);

	pthread_create(&t0, NULL, taska, NULL);
	pthread_create(&t1, NULL, taskb, NULL);
	pthread_create(&t2, NULL, taskc, NULL);
	pthread_create(&t3, NULL, taskd, NULL);
	pthread_create(&t4, NULL, taske, NULL);

	pthread_exit(NULL);

	return(0);
}
