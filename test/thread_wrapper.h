#ifndef THREAD_WRAPPER_H
#define THREAD_WRAPPER_H

#include <pthread.h> // Include the pthread library for threading functionality

typedef pthread_t Thread; // Define a type alias for pthread_t as Thread

// Function to create a new thread
void createThread(Thread *thread, void *(*start_routine)(void *), void *arg);

// Function to wait for a thread to terminate
void joinThread(Thread thread);

// Function to terminate the calling thread
void exitThread();

#endif /* THREAD_WRAPPER_H */
