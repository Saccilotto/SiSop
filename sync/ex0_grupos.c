#include <pthread.h>
#include <stdio.h>

pthread_mutex_t m1, m2, m3, m4, m5, m6;

void *task1(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m1);
		printf("task 1\n");
		pthread_mutex_unlock(&m2);
	}
}

void *task2(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m2);
		printf("task 2\n");
		pthread_mutex_unlock(&m3);
	}
}

void *task3(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m3);
		printf("task 3\n");
		pthread_mutex_unlock(&m1);
	}
}

void *task4(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m4);
		printf("task 4\n");
		pthread_mutex_unlock(&m5);
	}
}

void *task5(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m5);
		printf("task 5\n");
		pthread_mutex_unlock(&m6);
	}
}

void *task6(void *arg)
{
	while (1) {
		pthread_mutex_lock(&m6);
		printf("task 6\n");
		pthread_mutex_unlock(&m4);
	}
}

int main(int argc, char *argv[])
{
	pthread_t threads1[3];
    pthread_t threads2[3];

	pthread_mutex_init(&m1, NULL);
	pthread_mutex_init(&m2, NULL);
	pthread_mutex_init(&m3, NULL);
	pthread_mutex_lock(&m2);
	pthread_mutex_lock(&m3);

    pthread_mutex_init(&m4, NULL);
	pthread_mutex_init(&m5, NULL);
	pthread_mutex_init(&m6, NULL);
	pthread_mutex_lock(&m5);
	pthread_mutex_lock(&m6);

	pthread_create(&threads1[0], NULL, task1, NULL);
	pthread_create(&threads1[1], NULL, task2, NULL);
	pthread_create(&threads1[2], NULL, task3, NULL);

    pthread_create(&threads2[0], NULL, task4, NULL);
	pthread_create(&threads2[1], NULL, task5, NULL);
	pthread_create(&threads2[2], NULL, task6, NULL);
	
	pthread_exit(NULL);
	
	return 0;
}
