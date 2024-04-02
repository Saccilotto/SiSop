#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N_READERS	3
#define N_WRITERS	1

sem_t mutex_rc, mutex_wc, mutex, w_db, r_db;
int rc = 0, wc = 0, reads = 0, writes = 0;

void *reader(void *arg)
{
	long int i;

	i = (long int)arg;
	while (1) {
		sem_wait(&mutex);
		sem_wait(&r_db);
		sem_wait(&mutex_rc);
		rc++;
		if (rc == 1) sem_wait(&w_db);
		sem_post(&mutex_rc);
		sem_post(&r_db);
		sem_post(&mutex);
		reads++;
		printf("(R) thread %d reading the database... (%d readers, %d reads, %d writes)\n", i, rc, reads, writes);
		sem_wait(&mutex_rc);
		rc--;
		if (rc == 0) sem_post(&w_db);
		sem_post(&mutex_rc);
		printf("(R) thread %d using data...\n", i);
	}
}

void *writer(void *arg)
{
	long int i;

	i = (long int)arg;

	while(1) {
		sem_wait(&mutex_wc);
		wc++;
		if (wc == 1) sem_wait(&r_db);
		sem_post(&mutex_wc);
		printf("(W) thread %d preparing data...\n", i);
		sem_wait(&w_db);
		writes++;
		printf("(W) thread %d writing to the database... (%d reads, %d writes)\n", i, reads, writes);
		sem_post(&w_db);
		sem_wait(&mutex_wc);
		wc--;
		if (wc == 0) sem_post(&r_db);
		sem_post(&mutex_wc);
	}
}

int main(void)
{
	long int i;
	pthread_t readers[N_READERS], writers[N_WRITERS];

	sem_init(&mutex_rc, 0, 1);
	sem_init(&mutex_wc, 0, 1);	
	sem_init(&mutex, 0, 1);
	sem_init(&w_db, 0, 1);
	sem_init(&r_db, 0, 1);

	for (i = 0; i < N_READERS; i++)
		pthread_create(&readers[i], NULL, reader, (void *)i);

	for (i = 0; i < N_WRITERS; i++)
		pthread_create(&writers[i], NULL, writer, (void *)i);

	pthread_exit(NULL);

	return(0);
}
