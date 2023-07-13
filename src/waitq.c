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


