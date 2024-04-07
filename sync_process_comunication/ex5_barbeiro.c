#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N_BARBERS	3
#define N_CUSTOMERS	10
#define N_CHAIRS	5

sem_t customers, barbers, mutex;
int waiting = 0;

void *barber(void *arg)
{
	long int i;

	i = (long int)arg;
	
	while (1) {
		printf("(B) [thread %d] barber is sleeping...\n", i, waiting);
		sem_wait(&customers);
		sem_wait(&mutex);
		waiting--;
		sem_post(&barbers);
		sem_post(&mutex);
		printf("(B) [thread %d] cutting hair...\n", i, waiting);
	}
}

void *customer(void *arg)
{
	long int i;

	i = (long int)arg;

	while (1) {
		sem_wait(&mutex);
		if (waiting < N_CHAIRS) {
			printf("(C) [thread %d] sits in a vacant chair...\n", i);
			waiting++;
			sem_post(&customers);
			sem_post(&mutex);
			sem_wait(&barbers);
			printf("(C) [thread %d] getting the hair cut...\n", i);
		} else {
			sem_post(&mutex);
//			printf("(C) [thread %d] shop is full!\n", i);
		}
	}
}

int main(void)
{
	long int i;
	pthread_t tbarbers[N_BARBERS], tcustomers[N_CUSTOMERS];
	
	sem_init(&customers, 0, 0);
	sem_init(&barbers, 0, 0);
	sem_init(&mutex, 0, 1);

	for (i = 0; i < N_BARBERS; i++)
		pthread_create(&tbarbers[i], NULL, barber, (void *)i);

	for (i = 0; i < N_CUSTOMERS; i++)
		pthread_create(&tcustomers[i], NULL, customer, (void *)i);

	pthread_exit(NULL);	

	return(0);
}
