#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N_READERS	3
#define N_WRITERS	1

sem_t mutex, db;
int rc = 0, reads = 0, writes = 0;

void *reader(void *arg)
{
	long int i;

	i = (long int)arg;

	while (1) {
		sem_wait(&mutex);
		rc++;
		if (rc == 1) sem_wait(&db);
		sem_post(&mutex);
		reads++;
		printf("(R) thread %d reading the database... (%d readers, %d reads, %d writes)\n", i, rc, reads, writes);
		sem_wait(&mutex);
		rc--;
		if (rc == 0) sem_post(&db);
		sem_post(&mutex);
		printf("(R) thread %d using data...\n", i);
	}
}

void *writer(void *arg)
{
	long int i;

	i = (long int)arg;

	while (1) {
		printf("(W) thread %d preparing data...\n", i);
		sem_wait(&db);
		writes++;
		printf("(W) thread %d writing to the database... (%d reads, %d writes)\n", i, reads, writes);
		sem_post(&db);
	}
}

int main(void)
{
	long int i;
	pthread_t readers[N_READERS], writers[N_WRITERS];
	
	sem_init(&mutex, 0, 1);
	sem_init(&db, 0, 1);

	for (i = 0; i < N_READERS; i++)
		pthread_create(&readers[i], NULL, reader, (void *)i);

	for (i = 0; i < N_WRITERS; i++)
		pthread_create(&writers[i], NULL, writer, (void *)i);

	pthread_exit(NULL);	

	return(0);
}
