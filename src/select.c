
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "types.h"
#include "select.h"
#include "waitq.h"
#include "global.h"
#include "chan.h"
#include "lock.h"
#include "atomic.h"





void tprintf(const char *format, ...) {
    va_list args;
    pthread_t tid = pthread_self(); // obtiene el ID del hilo actual

    printf("[Thread %lu]: ", (unsigned long) tid); // imprime el ID del hilo

    va_start(args, format);
    vprintf(format, args); // imprime el resto del mensaje
    va_end(args);
}

void dump_waitq(waitq_t *queue) {
    waitq_node_t *current = queue->head; // Empieza en la cabeza de la cola
    while (current != NULL) { // Mientras no lleguemos al final de la cola
        printf("%p -> ", (void *)current->ptrcv); // Imprime la dirección de ptrcv
        current = current->next; // Avanza al siguiente nodo
    }
    printf("NULL\n"); // Indica el final de la cola
}

/*
 * Function: shuffle_select_set
 * ----------------------------
 * This function takes a set of channel operations and "shuffles" 
 * them to simulate the non-deterministic semantics of Go's select function. 
 * It uses the Fisher-Yates algorithm which is an efficient and correct way to do this.
 *
 * Parameters:
 *    set: A pointer to the set of channel operations to be shuffled.
 *    n:   The number of operations in the set.
 *
 * Returns:
 *    Nothing.
 */
static void shuffle_select_set(select_set_t *set, size_t n) {
    select_set_t tmp;
    size_t i;
    size_t j;
    
    if (n > 1) {
        srand(time(NULL));  // Seed the random number generator
        for (i = n - 1; i > 0; i--) {
            j = rand() % (i + 1);  // Generate a random index
            tmp = set[j];  // Swap elements
            set[j] = set[i];
            set[i] = tmp;
        }
    }
}

/*
 * Function: wakeup_next_waiting
 * ----------------------------
 * This function wakes up the next waiting thread that's associated with a specific 'chan_t'. 
 *
 * This function accomplishes its task by dequeuing a condition variable from the queue associated 
 * with the operation type (op_type), which could either be OP_SEND or OP_RECV. It then performs an 
 * atomic compare-and-exchange operation to assign a new value to cv->cd if its current value is 
 * CV_NULL_CHANNEL_DESCRIPTOR. If the operation is successful, the function wakes up the thread associated 
 * with the condition variable and decreases its reference count. 
 *
 * If the compare-and-exchange operation is not successful, the function decreases the reference count 
 * of the condition variable. If its reference count goes to 0, the condition variable is released.
 *
 * Parameters:
 * - chan: A pointer to the 'chan_t' structure associated with the waiting threads.
 * - op_type: The type of operation (OP_SEND or OP_RECV) to perform.
 * - cd: An integer representing the condition variable's descriptor.
 *
 * Returns: void
 */
static void wakeup_next_waiting(chan_t *chan, int op_type, int cd) {
    condvar_t *cv;
    pthread_t *thread;
    int end = 0;

    // Initialize expected value for atomic_compare_exchange_strong
    int expected = CV_NULL_CHANNEL_DESCRIPTOR;

    // Continue the loop until there are no more condition variables to dequeue
    while(!end) {
        // Dequeue the condition variable from the appropriate queue based on the operation type
        if (op_type == OP_SEND && chan->recv_shift == NULL) {
            cv = dequeue(&(chan)->recvq);
        } else {
            if (op_type == OP_RECV && chan->send_shift == NULL ) {
                cv = dequeue(&(chan)->sendq);
            } else {
                cv = NULL;
            }
        }
        // Break the loop if there's no condition variable
        if (!cv) {
            end = 1;
            continue;
        }

        // If cv->cd == expected (which is CV_NULL_CHANNEL_DESCRIPTOR), then cv->cd = cd
        // Also, if the exchange was successful, do the following:
        if (atomic_compare_exchange_strong(&(cv->cd), &expected, cd)) {
            // Lock the condition variable's mutex to avoid race conditions
            pthread_mutex_lock(&(cv->mutex));
            // Decrease the reference count of the condition variable
            ATOMIC_INC(&(cv->ref));
            // Allocate space for the thread
            thread  = calloc(1, sizeof(pthread_t));
            // Copy the thread from the condition variable
            *thread = cv->thread;
            // Depending on the operation type, assign the thread to the appropriate field in the channel
            if (op_type == OP_SEND) 
                chan->recv_shift = thread;
            else
                chan->send_shift = thread;

            // Signal the condition variable's condition
            pthread_cond_signal(&(cv->pcond));
            // Unlock the condition variable's mutex
            pthread_mutex_unlock(&(cv->mutex));
            end = 1;

        } else {
            // If cv->cd != expected, decrease the reference count of the condition variable
            // If the reference count becomes 0, release the condition variable
            if (ATOMIC_DEC(&(cv->ref)) == 0) {
                release_condvar(&cv);
            }
        }
    }
}


/*
 * Function: select_chan_try_op
 * ----------------------------
 * This function tries to perform an operation (send or receive) on a channel.
 * It first determines the appropriate wait queue based on the operation type.
 * Then, depending on the operation type and the current thread (represented by the shift variable), 
 * it attempts to write to or read from the channel buffer.
 * If the operation is successful, the function returns 1, otherwise it returns 0.
 *
 * Parameters:
 * chan: A pointer to the channel structure on which the operation is to be performed.
 * op_type: An integer representing the type of operation to perform. OP_SEND for a send operation, OP_RECV for a receive operation.
 * data: A void pointer to the data to be sent or the location where the received data should be stored.
 *
 * Returns:
 * 1 if the operation was successful, and 0 otherwise.
 */
static int select_chan_try_op(chan_t *chan, int op_type, void *data) {
//    waitq_t *wqueue = (op_type == OP_SEND) ? &(chan->sendq) : &(chan->recvq);
    any_t   *value  = data;
    int ok;

    /* At this point I can try to send or recv because I'm the first or 
       there are not requests on this channel with the same operation.
       Depending on the operation type, try to send or receive data. */
    if (op_type == OP_SEND) {
        /* Try to send data to the channel. */
        if (chan->send_shift == NULL || (chan->send_shift && *(chan->send_shift) == pthread_self())) {
            ok = cb_write(chan->cb, *value);
            if (ok && chan->send_shift != NULL) {
                free(chan->send_shift);
                chan->send_shift = NULL;
            }
        } else {
            ok = 0;
        }
    } else {
        /* Try to receive data from the channel. */
        if (chan->recv_shift == NULL || (chan->recv_shift && *(chan->recv_shift) == pthread_self())) {
            ok = cb_read(chan->cb, value);
            if (ok && chan->recv_shift != NULL) {
                free(chan->recv_shift);
                chan->recv_shift = NULL;
            }
        } else {
            ok = 0;
        }
    }
    if (ok) 
        return 1;
    return 0;
}

/*
 * Function: loockup_cd
 * --------------------
 * This function searches for a channel descriptor (cd) in a set of select_set_t structures. 
 * It iterates over each select_set_t in the set and compares its cd with the provided cd. 
 * If it finds a match, it returns the index of the matching select_set_t in the set. 
 * If it does not find a match, it returns -1.
 *
 * Parameters:
 * set: A pointer to the set of select_set_t structures. Each select_set_t contains a channel descriptor (cd), 
 *      the operation type, and possibly the send and receive values.
 * size: The size of the set, i.e., the number of select_set_t in the set.
 * cd: The channel descriptor that is being searched in the set.
 *
 * Returns:
 * The index in the set where cd was found, or -1 if cd is not found.
 */

static int loockup_cd(select_set_t *set, int size, int cd) {
    for (int i = 0; i < size; i++) {
        if (set[i].cd == cd) {
            return i;
        }
    }
    // Return -1 if no matching cd is found.
    return -1;
}

void print_array(select_set_t *array, size_t n) {
    for (size_t i = 0; i < n; i++) {
        tprintf("Estructura %zu - cd: %d, op_type: %d\n", i, array[i].cd, array[i].op_type);
    }
}

void dump_channel(int cd) {
    chan_t *channel = get_channel_from_table(cd);
    if (channel == NULL) {
        printf("Channel is NULL\n");
        return;
    }

    printf("Channel info:\n");
    printf("--------------------\n");

    // Print mutex info
    printf("Mutex: %p\n", (void*)&(channel->mutex));

    // Print recvq info
    printf("RecvQ count: %d\n", channel->recvq.len);
    printf("RecvQ head: %p\n", (void*)channel->recvq.head);
    printf("RecvQ tail: %p\n", (void*)channel->recvq.tail);

    // Print sendq info
    printf("SendQ count: %d\n", channel->sendq.len);
    printf("SendQ head: %p\n", (void*)channel->sendq.head);
    printf("SendQ tail: %p\n", (void*)channel->sendq.tail);
    printf("Recv Shift: %lu\n", channel->recv_shift ? (unsigned long) *(channel->recv_shift): 0);
    printf("Send Shift: %lu\n", channel->send_shift ? (unsigned long) *(channel->send_shift): 0);
    // Print circular buffer info
    if (channel->cb != NULL) {
        printf("Circular Buffer info:\n");
        printf("Start: %d\n", channel->cb->start);
        printf("End: %d\n", channel->cb->end);
        printf("Len: %lu\n", channel->cb->len);
        printf("Cap: %lu\n", channel->cb->cap);
        printf("Buff: %p\n", (void*)channel->cb->buff);
    } else {
        printf("Circular Buffer is NULL\n");
    }
}


/*
 * Function: wait_and_release
 * ---------------------------
 * This function makes the current thread wait until a condition variable is set
 * to a value different from CV_NULL_CHANNEL_DESCRIPTOR. If the reference count
 * of the condition variable drops to 0 during this waiting period, it releases 
 * the associated resources. After the waiting period is over, the function returns 
 * the channel descriptor associated with the condition variable.
 *
 * cvar: double pointer to the condition variable structure. 
 *
 * returns: the channel descriptor associated with the condition variable.
 */
static int wait_and_release(condvar_t **cvar) {
    int cd;

    // Acquire the mutex lock
    pthread_mutex_lock(&((*cvar)->mutex));
    // Wait until the channel descriptor is not CV_NULL_CHANNEL_DESCRIPTOR.
    // If during this period the reference count drops to zero, release the condition variable resource
    while ((cd = atomic_load(&((*cvar)->cd))) == CV_NULL_CHANNEL_DESCRIPTOR) {
        pthread_cond_wait(&((*cvar)->pcond), &((*cvar)->mutex));
    }
    if (ATOMIC_DEC(&((*cvar)->ref)) == 0) {
        // At this point, it is safe to release the condition variable as no other thread is using it
        pthread_mutex_unlock(&((*cvar)->mutex));
        release_condvar(cvar);
    } else {
        pthread_mutex_unlock(&((*cvar)->mutex));
    }
    // Release the mutex lock
    

    // Return the channel descriptor
    return cd;
}



/*
 * Function: select_chan_op
 * ------------------------
 * This function attempts to synchronize and communicate between threads
 * using a select operation on multiple channels.
 *
 * Parameters:
 * - set: a pointer to an array of `select_set_t` structures, representing the channels to be selected.
 * - n: the number of channels in the array.
 * - should_block: a int to check if the select needs to wait until one channel is ready or not
 *
 * Returns:
 * - Upon successful completion, the function returns 1.
 * - If there are no channels in the array (n == 0), the function returns -1.
 * - If the select operation is unsuccessful, the function goes into a blocking state, waiting for a condition to be signaled.
 * - The function returns the result of `select_chan_loop` function, if the condition variable is signaled.
 *
 * Notes:
 * - This function uses two loops. The first loop attempts to perform the select operation on the channels. If unsuccessful,
 *   it increments the reference count of the condition variable and enqueues the condition variable to the appropriate queue
 *   based on the operation type (send or receive).
 * - After all channels have been processed, the function releases all locks using `unlockall`.
 * - It then enters a blocking state, waiting for a condition to be signaled.
 * - Once signaled, it obtains the index of the signaled condition variable and performs the select operation again on that channel.
 */
int select_chan_op(select_set_t *set, size_t n, int should_block) {
    select_set_t *pset;
    chan_t       *chan;
    waitq_t      *queue;
    condvar_t    *cvar;
    int i;
    int cd;
    int success;
    int *lockorder;

    // If no operations are specified, return -1
    if (n == 0)
        return 0;

    // If there's more than one operation, shuffle them to avoid bias
    if (n > 1)
        shuffle_select_set(set, n);

    // Lock all the channels in ascending order to prevent deadlocks
    lockorder = lockall(set, n);

    // Try to perform all operations without blocking
    for (i = 0; i < n; i++) {
        pset = &set[i];
        chan = get_channel_from_table(pset->cd);

        // Check if channel is closed or does not exist
        if (!chan) {
            unlockall(&lockorder, n);
            return -(pset->cd);
        }

        // Try to perform the operation
        if ((success = select_chan_try_op(chan, pset->op_type, (pset->op_type == OP_SEND) ? pset->send : pset->recv))) {
            // If the operation was successful, wake up the next thread waiting for the opposite operation
            wakeup_next_waiting(chan, pset->op_type, pset->cd);
            // Unlock all the channels
            unlockall(&lockorder, n);
            // Return success
            return pset->cd;
        }
    }

    // If should_block is false, we unlock all channels and return the result of the operations (successful or not)
    if (!should_block) {
        unlockall(&lockorder, n);
        return 0;
    }

    // If should_block is true and no operation was successful, we block until an operation can be performed

    // Create a new condition variable and set its thread to the current one
    cvar = empty_condvar();
    cvar->thread = pthread_self();
    ATOMIC_INC(&(cvar->ref));
    // Enqueue the condition variable in the waiting queue of each operation
    for (i = 0; i < n; i++) {
        pset = &set[i];
        chan = get_channel_from_table(pset->cd);

        // Increase the reference count of the condition variable
        ATOMIC_INC(&(cvar->ref));
        // Enqueue the condition variable in the appropriate queue
        if (pset->op_type == OP_SEND)
            enqueue(&(chan->sendq), cvar);
        else
            enqueue(&(chan->recvq), cvar);
    }

    // Unlock all the channels
    unlockall(&lockorder, n);

    // Wait until one of the operations can be performed and get the channel descriptor of the operation
    cd = wait_and_release(&cvar);
    // Find the index of the operation that can be performed
    i = loockup_cd(set, n, cd);
    // Try to perform the operation again
    return select_chan_op(&set[i], 1, should_block);
}


/*
 * Function: select_chan_op
 * ------------------------
 * This function attempts to synchronize and communicate between threads
 * using a select operation on multiple channels.
 *
 * Parameters:
 * - set: a pointer to an array of `select_set_t` structures, representing the channels to be selected.
 * - n: the number of channels in the array.
 * - should_block: a int to check if the select needs to wait until one channel is ready or not
 *
 * Returns:
 * - Upon successful completion, the function returns 1.
 * - If there are no channels in the array (n == 0), the function returns -1.
 * - If the select operation is unsuccessful, the function goes into a blocking state, waiting for a condition to be signaled.
 * - The function returns the result of `select_chan` function, if the condition variable is signaled.
 */
int select_chan(select_set_t *set, size_t n, int should_block) {
    return select_chan_op(set, n, should_block);
} 

/*
 * Function: send_chan
 * -------------------
 * This function sends data to a channel.
 *
 * Parameters:
 *    cd   - the descriptor of the channel to send to.
 *    send - a pointer to the data to send.
 *
 * Returns:
 *    On success, it returns the descriptor of the channel where the data was sent.
 *    If the operation was not successful, it returns -1.
 */
int send_chan(int cd, any_t *send) {
    select_set_t op[] = {
        {cd, OP_SEND, send, NULL},  // Define a channel operation for sending.
    };
    return select_chan(op, 1, 1);  // Attempt to perform the operation.
}

/*
 * Function: recv_chan
 * -------------------
 * This function receives data from a channel.
 *
 * Parameters:
 *    cd   - the descriptor of the channel to receive from.
 *    recv - a pointer to a location where the received data should be stored.
 *
 * Returns:
 *    On success, it returns the descriptor of the channel from which the data was received.
 *    If the operation was not successful, it returns -1.
 */
int recv_chan(int cd, any_t *recv) {
    select_set_t op[] = {
        {cd, OP_RECV, NULL, recv},  // Define a channel operation for receiving.
    };
    return select_chan(op, 1, 1);  // Attempt to perform the operation.
}