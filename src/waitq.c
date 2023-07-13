#include <stdlib.h>
#include "waitq.h"
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
int enqueue(waitq_t *waitq, condvar_t *ptrcv) {
    waitq_node_t *new_node = malloc(sizeof(waitq_node_t));
    if (!new_node) {
        return -1;
    }

    new_node->ptrcv = ptrcv;
    new_node->next = NULL;

    if (waitq->len == 0) {
        waitq->head = new_node;
        new_node->prev = NULL;
    } else {
        waitq->tail->next = new_node;
        new_node->prev = waitq->tail;
    }
    waitq->tail = new_node;
    waitq->len++;
    return 0;
}

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
condvar_t *dequeue(waitq_t *waitq) {
    condvar_t *ptrcv = NULL;
    waitq_node_t *head;

    if (waitq->len > 0 ) {
        head = waitq->head;
        ptrcv = head->ptrcv;
        waitq->head = head->next;
        if (waitq->head) {
            waitq->head->prev = NULL;
        } else {
            waitq->tail = NULL;
        }
        waitq->len--;
        free(head);
    }
    return ptrcv;
}


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
condvar_t *alloc_condvar() {
    condvar_t *cv = calloc(1, sizeof(condvar_t));
    if (!cv) {
        return NULL;
    }

    // Initialize the condition variable and mutex
    pthread_cond_init(&(cv->pcond), NULL);
    pthread_mutex_init(&(cv->mutex), NULL);
    atomic_init(&(cv->ref), 0);
    atomic_init(&(cv->cd), CV_NULL_CHANNEL_DESCRIPTOR);
    // Other members of condvar_t can be initialized here as needed.

    return cv;
}

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
void free_condvar(condvar_t **cv) {
    if (!cv || !*cv) {
        return;
    }

    // Destroy the condition variable and mutex before freeing the memory
    pthread_cond_destroy(&((*cv)->pcond));
    pthread_mutex_destroy(&((*cv)->mutex));

    free(*cv);
    *cv = NULL;
}
