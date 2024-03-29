#include "peterson.h"

// Function to acquire the Peterson lock
void peterson_lock(PetersonMutex *mutex, int id)
{
    int other = 1 - id;
    mutex->flag[id] = true;
    mutex->turn = id;
    while (mutex->flag[other] && mutex->turn == id)
        ; // Busy wait
}

// Function to release the Peterson lock
void peterson_unlock(PetersonMutex *mutex, int id)
{
    mutex->flag[id] = false;
}

// Function to initialize the Peterson lock
void init_peterson(PetersonMutex *mutex)
{
    mutex->flag[0] = mutex->flag[1] = false;
    mutex->turn = 0;
}
