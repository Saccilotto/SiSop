#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "peterson.h"
#include "thread_wrapper.h"

#define NUM_CANIBALS 5
#define PORTION_SIZE 3

int servings = 0;    // Global variable to count remaining servings
PetersonMutex mutex; // Global Peterson mutex

// Function for canibals
void *canibal(void *arg)
{
    int id = *((int *)arg);
    while (1)
    {
        if (servings == 0)
        {
            // If the serving dish is empty, wake up the chef
            peterson_lock(&mutex, id);
            printf("Canibal %d: Waking up the chef\n", id);
            peterson_unlock(&mutex, id);
        }

        // Try to acquire the mutex
        peterson_lock(&mutex, id);

        if (servings > 0)
        {
            // If there are remaining servings, serve oneself
            servings--;
            printf("Canibal %d: Serving oneself. Remaining servings: %d\n", id, servings);
        }
        else
        {
            // If there are no remaining servings, release the mutex and wait
            printf("Canibal %d: Serving dish empty. Waiting...\n", id);
            peterson_unlock(&mutex, id);
            sleep(2);
            continue;
        }

        // Release the mutex
        peterson_unlock(&mutex, id);

        // Eat
        sleep(rand() % 3 + 1);
    }
}

int main()
{
    pthread_t canibals[NUM_CANIBALS];
    int ids[NUM_CANIBALS];

    // Initialize the mutex
    init_peterson(&mutex);

    // Initialize the threads for canibals
    for (int i = 0; i < NUM_CANIBALS; ++i)
    {
        ids[i] = i;
        pthread_create(&canibals[i], NULL, canibal, &ids[i]);
    }

    // Wait for the threads to finish
    for (int i = 0; i < NUM_CANIBALS; ++i)
    {
        pthread_join(canibals[i], NULL);
    }

    return 0;
}
