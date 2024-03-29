#ifndef PETERSON_H
#define PETERSON_H

#include <stdbool.h>

// Structure representing the Peterson mutex
typedef struct
{
    bool flag[2];
    int turn;
} PetersonMutex;

// Function prototypes
void peterson_lock(PetersonMutex *mutex, int id);
void peterson_unlock(PetersonMutex *mutex, int id);
void init_peterson(PetersonMutex *mutex);

#endif /* PETERSON_H */
