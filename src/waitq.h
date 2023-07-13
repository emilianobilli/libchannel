#ifndef _LC_WAITQ_H
#define _LC_WAITQ_H 1

#include <pthread.h>
#include <stdatomic.h>

#define CV_NULL_CHANNEL_DESCRIPTOR -1

/* 
 * `condvar_t` is a structure that bundles a condition variable and a mutex, 
 * along with a boolean indicating whether it's a select operation, and a channel descriptor.
 */
typedef struct {
    pthread_cond_t  pcond;    // Condition variable for thread synchronization.
    pthread_mutex_t mutex;       // Mutex to ensure mutual exclusion.
    pthread_t  thread;
    atomic_int ref;
    atomic_int cd;
} condvar_t;

/* 
 * `waitq_node_t` is a structure representing a node in a doubly linked queue. 
 * It contains a pointer to a `condvar_t` structure and pointers to the next and previous node in the queue.
 */
typedef struct waitq_node {
    condvar_t    *ptrcv;      // Pointer to the associated condvar_t structure.
    struct waitq_node *next;     // Pointer to the next node in the queue.
    struct waitq_node *prev;     // Pointer to the previous node in the queue.
} waitq_node_t;

/* 
 * `waitq_t` is a structure representing a doubly linked queue. 
 * It contains a counter of the queue's length and pointers to the head and tail nodes.
 */
typedef struct waitq {
    int len;                     // Length of the queue.
    struct waitq_node *head;     // Pointer to the head node in the queue.
    struct waitq_node *tail;     // Pointer to the tail node in the queue.
} waitq_t;

/* 
 * Function: dequeue
 * -----------------
 * This function removes the first (head) node from the queue passed as an argument
 * and returns the `condvar_t` pointer that was stored in that node.
 * 
 * Parameters:
 *    waitq - a pointer to the queue from which the node is to be dequeued.
 * 
 * Returns:
 *    On success, it returns a pointer to a `condvar_t` structure that was stored in the dequeued node.
 *    If the queue is empty, it returns NULL.
 */
extern condvar_t *dequeue(waitq_t *waitq);

/* 
 * Function: enqueue
 * -----------------
 * This function creates a new node, stores the `condvar_t` pointer passed as an argument into the new node, 
 * and adds the new node to the end (tail) of the queue passed as an argument.
 * 
 * Parameters:
 *    waitq - a pointer to the queue to which the new node is to be enqueued.
 *    ptrcv - a pointer to a `condvar_t` structure to be stored in the new node.
 * 
 * Returns:
 *    On success, it returns 0.
 *    If the function fails to allocate memory for the new node, it returns -1.
 */
extern int enqueue(waitq_t *waitq, condvar_t *ptrcv);

/* 
 * Function: alloc_condvar
 * -----------------------
 * This function allocates memory for a new `condvar_t` structure.
 * 
 * Parameters:
 *    None.
 * 
 * Returns:
 *    On success, it returns a pointer to the newly allocated `condvar_t` structure.
 *    If the function fails to allocate memory, it returns NULL.
 */
extern condvar_t *alloc_condvar();

/* 
 * Function: free_condvar
 * ----------------------
 * This function releases the memory previously allocated for a `condvar_t` structure.
 * 
 * Parameters:
 *    cv - a pointer to the pointer of the `condvar_t` structure that is to be deallocated.
 * 
 * Returns:
 *    Nothing.
 */
extern void free_condvar(condvar_t **cv);


#endif
