#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "peterson.h"
#include "concurrent_queue.h"
#include "thread_wrapper.h"

#define NUM_PRODUCERS 4
#define NUM_CONSUMERS 4

ConcurrentQueue queue; // Queue variable declaration
PetersonMutex mutex;   // Peterson mutex variable declaration

// Production function
void *producer(void *arg)
{
    int id = *((int *)arg);
    for (int i = 0; i < 5; ++i)
    {
        // Produce an item
        int item = rand() % 100;

        // Tries to acquire the mutex for queue insertion
        peterson_lock(&mutex, id);

        // Inserts the item into the queue
        enqueue(&queue, item);
        printf("Producer %d produced item %d\n", id, item);

        // Releases the mutex after queue insertion
        peterson_unlock(&mutex, id);

        // Sleeps for a short period of time
        sleep(1);
    }
    return NULL;
}

// Main function
int main()
{
    // Initializes the queue
    initQueue(&queue);

    // Initializes the mutex
    mutex.flag[0] = false;
    mutex.flag[1] = false;
    mutex.turn = 0;

    // Initializes the threads
    pthread_t producers[NUM_PRODUCERS];
    int ids[NUM_PRODUCERS];

    for (int i = 0; i < NUM_PRODUCERS; ++i)
    {
        ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    }

    // Waits for the threads to finish
    for (int i = 0; i < NUM_PRODUCERS; ++i)
    {
        pthread_join(producers[i], NULL);
    }

    return 0;
}
