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
    int  closed;
    
    pthread_mutex_t mutex;

    pthread_t *recv_shift;
    pthread_t *send_shift;
    
    waitq_t recvq;
    waitq_t sendq;
} chan_t;

#endif