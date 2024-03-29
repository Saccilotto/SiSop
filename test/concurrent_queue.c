#include <stdbool.h>

// Define the size of the queue
#define QUEUE_SIZE 10

// Structure to represent a concurrent queue
typedef struct
{
    int buffer[QUEUE_SIZE]; // Array to store queue elements
    int front, rear;        // Pointers to track the front and rear of the queue
    int count;              // Count of elements in the queue
} ConcurrentQueue;

// Function to initialize the queue
void initQueue(ConcurrentQueue *queue)
{
    queue->front = 0; // Set front pointer to the beginning of the array
    queue->rear = -1; // Set rear pointer to -1 (empty queue)
    queue->count = 0; // Set count of elements to 0
}

// Function to check if the queue is full
bool isFull(ConcurrentQueue *queue)
{
    return queue->count == QUEUE_SIZE; // Return true if count equals the queue size
}

// Function to check if the queue is empty
bool isEmpty(ConcurrentQueue *queue)
{
    return queue->count == 0; // Return true if count equals 0
}

// Function to add an element to the queue
void enqueue(ConcurrentQueue *queue, int data)
{
    // Move rear pointer circularly to the next position
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    // Add the new element to the queue
    queue->buffer[queue->rear] = data;
    // Increment the count of elements in the queue
    queue->count++;
}

// Function to remove an element from the queue
int dequeue(ConcurrentQueue *queue)
{
    // Get the data from the front of the queue
    int data = queue->buffer[queue->front];
    // Move front pointer circularly to the next position
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    // Decrement the count of elements in the queue
    queue->count--;
    // Return the removed data
    return data;
}
