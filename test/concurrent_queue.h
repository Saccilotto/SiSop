#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <stdbool.h>

#define QUEUE_SIZE 10

typedef struct
{
    int buffer[QUEUE_SIZE]; // Array to store queue elements
    int front, rear;        // Front and rear pointers
    int count;              // Number of elements in the queue
} ConcurrentQueue;

// Initializes the queue
void initQueue(ConcurrentQueue *queue);

// Checks if the queue is full
bool isFull(ConcurrentQueue *queue);

// Checks if the queue is empty
bool isEmpty(ConcurrentQueue *queue);

// Inserts an element into the queue
void enqueue(ConcurrentQueue *queue, int data);

// Removes and returns the element from the queue
int dequeue(ConcurrentQueue *queue);

#endif /* CONCURRENT_QUEUE_H */
