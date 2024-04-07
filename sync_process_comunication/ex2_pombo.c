#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N	20

int postit = 0;
sem_t mutex, cheia, enchendo;

void *usuario(void *arg)
{
	long int i;

	i = (long int)arg;

	while (1) {
		sem_wait(&enchendo);
		sem_wait(&mutex);
		printf("\nusuario %d cola post it %d", i, postit);
		postit++;
		if (postit == N)
			sem_post(&cheia);
		sem_post(&mutex);
	}
}

void *pombo(void *arg)
{
	int i;

	while (1) {
		sem_wait(&cheia);
		sem_wait(&mutex);
		printf("\npombo levando a mochila e voltando...");
		postit = 0;
		for (i = 0; i < N; i++)
			sem_post(&enchendo);
		sem_post(&mutex);
	}
}

int main(void)
{
	long int i;
	pthread_t tpombo, tusuario[N];
	
	sem_init(&mutex, 0, 1);
	sem_init(&cheia, 0, 0);
	sem_init(&enchendo, 0, N);

	pthread_create(&tpombo, NULL, pombo, (void *)0);
	for (i = 1; i <= N; i++)
		pthread_create(&tusuario[i-1], NULL, usuario, (void *)i);

	pthread_exit(NULL);
	
	return(0);
}
