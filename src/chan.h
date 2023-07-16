/*
 * File: chan.h
 * ----------------------------
 * This header file includes definitions for the channel structure used in the library.
 *
 * The 'chan_t' structure is the main structure that represents a channel. It includes 
 * pointers to 'cbuff_t' (circular buffer), 'recv_shift' and 'send_shift' for the receiving 
 * and sending threads respectively. It also contains two wait queues 'recvq' and 'sendq' 
 * for the receiving and sending operations respectively. Furthermore, it includes a mutex 
 * to ensure safe concurrent access.
 *
 * The file also includes the necessary headers for various types, circular buffer, 
 * and wait queue used in the library.
 *
 * Structures:
 * chan_t: The structure representing a channel.
 *
 *      cbuff_t *cb: A pointer to the circular buffer of the channel.
 *      pthread_mutex_t  mutex: A mutex for the channel to ensure safe concurrent access.
 *      pthread_t *recv_shift: A pointer to the receiving thread.
 *      pthread_t *send_shift: A pointer to the sending thread.
 *      waitq_t recvq: A wait queue for the receiving operations.
 *      waitq_t sendq: A wait queue for the sending operations.
 *
 * Note:
 * chan.h should only be included once, hence the use of '_LC_CHAN_' definition to 
 * prevent multiple inclusions.
 */
#ifndef _LC_CHAN_
#define _LC_CHAN_ 1


#define CV_NULL_CHANNEL_DESCRIPTOR -1

#include <pthread.h>

#include "types.h"
#include "cb.h"
#include "waitq.h"

typedef struct {
    cbuff_t *cb;    
    pthread_mutex_t mutex;

    pthread_t *recv_shift;
    pthread_t *send_shift;
    
    waitq_t recvq;
    waitq_t sendq;
} chan_t;

/*
 * Function: new_chan
 * ---------------------
 * Initialize a new channel of given length.
 *
 * Parameters:
 * len: the length for the channel's internal buffer.
 *
 * Returns: a pointer to the newly allocated channel. If memory allocation 
 * fails, returns NULL.
 *
 */
extern chan_t *new_chan(size_t);

/*
 * Function: del_chan
 * ---------------------
 * Delete a channel and free its associated resources. 
 *
 * Parameters:
 * chan: a pointer to the channel to be deleted.
 *
 * Returns: nothing.
 *
 */
extern void del_chan(chan_t *chan);

/*
 * Function: is_closeable
 * ------------------------
 * Check if a channel can be closed. 
 *
 * A channel is closeable if there are no shifts waiting to send or receive 
 * from it and if its send and receive queues are empty.
 *
 * Parameters:
 * chan: a pointer to the channel to be checked.
 *
 * Returns: 1 if the channel is closeable and 0 otherwise.
 *
 */
extern int is_closeable(chan_t *chan);


#endif