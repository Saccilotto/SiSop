# Synchronization Problems with Threads in C

This repository contains C implementations to solve two thread synchronization problems: Producers and Consumers and the Cannibals' Dinner. The solutions use semaphores and mutexes to ensure synchronization between threads.

## Problem 1: Producers and Consumers

In this problem, multiple producer threads add items to a shared queue, and multiple consumer threads remove items from this queue. The queue is implemented as a circular array. The solution uses a mutex to ensure exclusive access to the queue and semaphores to control the number of items in the queue.

### Files
- `producer_consumer.c`: Implementation of the Producers and Consumers problem.
- `concurrent_queue.c`: Implementation of the shared queue.
- `peterson.c`: Implementation of the Peterson algorithm to ensure mutual exclusion.
- `thread_wrapper.c`: Auxiliary functions for thread creation and execution.

## Problem 2: Cannibals' Dinner

In this problem, a group of cannibals shares a platter with a limited number of food portions. Cannibals can serve themselves from the platter simultaneously, but when the platter is empty, they wake up the cook to refill it. The solution uses semaphores to control access to the platter.

### Files
- `problem2.c`: Implementation of the Cannibals' Dinner problem.
- `peterson.c`: Implementation of the Peterson algorithm to ensure mutual exclusion.
- `thread_wrapper.c`: Auxiliary functions for thread creation and execution.

## Compilation and Execution

To compile the programs, use the `make` command in the root directory. This will generate the `producer_consumer` and `problem2` executables. You can run these programs directly or pass arguments as needed.

## How to Contribute

Feel free to contribute improvements, bug fixes, or new features. Simply open a new issue or send a pull request.
