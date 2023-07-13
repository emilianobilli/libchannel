
#ifndef _LOCK_H
#define _LOCK_H 1

#include "select.h"

/*
 * Function: lockall
 * -----------------
 * This function acquires locks for all channels in the provided set in ascending order of their descriptors.
 * This is done to prevent deadlocks that could occur if multiple threads attempt to lock channels in different 
 * orders simultaneously. Before locking, it creates an array 'lockorder' to store channel descriptors 
 * (cd) and sorts them using 'qsort'. Then it locks all the channels based on this order. 
 * It returns a pointer to the 'lockorder' array.
 * 
 * Parameters:
 * set: A pointer to an array of 'select_set_t' structure
 * n: The size of the set
 *
 * Returns:
 * A pointer to the 'lockorder' array or NULL if memory allocation fails
 */
extern int *lockall(select_set_t *set, size_t n);

/*
 * Function: unlockall
 * -------------------
 * This function releases locks for all channels specified by the provided 'lockorder' array. 
 * It iterates through 'lockorder', unlocks each channel, then frees the memory allocated 
 * for 'lockorder' and sets the pointer to NULL to prevent any potential dangling pointer.
 * 
 * Parameters:
 * lockorder: A double pointer to the array of channel descriptors
 * n: The size of the 'lockorder' array
 *
 * Returns:
 * Void
 */
extern void unlockall(int **lockorder, size_t n);

#endif