#include <pthread.h> // Include the pthread library for threading functionality
#include <stdlib.h>  // Include the standard library for memory allocation functions

typedef pthread_t Thread; // Define a type alias for pthread_t as Thread

// Function to create a new thread
void createThread(Thread *thread, void *(*start_routine)(void *), void *arg)
{
    pthread_create(thread, NULL, start_routine, arg); // Call pthread_create to create a new thread
}

// Function to wait for a thread to terminate
void joinThread(Thread thread)
{
    pthread_join(thread, NULL); // Call pthread_join to wait for the specified thread to terminate
}

// Function to terminate the calling thread
void exitThread()
{
    pthread_exit(NULL); // Call pthread_exit to terminate the calling thread
}
