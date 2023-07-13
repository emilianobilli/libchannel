
#include <stdlib.h>
#include <stdio.h>

#include "select.h"
#include "chan.h"
#include "global.h"

/*
 * Function: compare_int
 * ---------------------
 * This function compares two integers pointed by 'a' and 'b'.
 * It is used as a comparator function by 'qsort'. 
 * The subtraction operation between the dereferenced pointer values allows this function 
 * to work as a comparator for both ascending and descending sorts. 
 * If the result of the subtraction is less than 0, 'a' is considered smaller than 'b'.
 * If it's 0, 'a' and 'b' are considered equal, and if it's greater than 0, 'a' is considered larger than 'b'.
 *
 * Parameters:
 * a: A void pointer to the first integer to be compared
 * b: A void pointer to the second integer to be compared
 *
 * Returns:
 * A negative value if '*a' < '*b', 0 if '*a' == '*b', or a positive value if '*a' > '*b'
 */
static int compare_int(const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}


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
int *lockall(select_set_t *set, size_t n) {
    int i;
    int *lockorder = calloc(n, sizeof(int));
    chan_t *chan;
    if (!lockorder)
        return NULL;

    for (i = 0; i < n; i++)
        lockorder[i] = set[i].cd;

    qsort(lockorder, n, sizeof(int), compare_int);

    for (i = 0; i < n; i++) {
        chan = get_channel_from_table(lockorder[i]);
        pthread_mutex_lock(&(chan->mutex));
    }
    return lockorder;
}

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
void unlockall(int **lockorder, size_t n) {
    int i;
    chan_t *chan;
    for (i = 0; i < n; i++) {
        chan = get_channel_from_table((*lockorder)[i]);
        pthread_mutex_unlock(&(chan->mutex));
    }
    free(*lockorder);
    *lockorder = NULL;
}
