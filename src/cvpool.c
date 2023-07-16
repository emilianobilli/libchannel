#include <stdlib.h>
#include "waitq.h"
#include "chan.h"

/*
 * Global Variable: condvar_pool
 * -----------------------------
 * A queue (waitq_t) that stores unused condition variable (condvar_t) structures.
 */
static waitq_t condvar_pool;

/*
 * Global Variable: condvar_pool_max
 * ---------------------------------
 * Maximum size for the condition variable pool.
 */
static int condvar_pool_max;

/*
 * Global Variable: condvar_pool_mutex
 * -----------------------------------
 * A mutex to handle concurrent access to the condition variable pool.
 */
static pthread_mutex_t condvar_pool_mutex;

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
static condvar_t *alloc_condvar() {
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
static void free_condvar(condvar_t **cv) {
    if (!cv || !*cv) {
        return;
    }

    // Destroy the condition variable and mutex before freeing the memory
    pthread_cond_destroy(&((*cv)->pcond));
    pthread_mutex_destroy(&((*cv)->mutex));

    free(*cv);
    *cv = NULL;
}

/*
 * Function: empty_condvar
 * -----------------------
 * Retrieves an empty condition variable. If the pool (condvar_pool) already contains unused condition variables,
 * this function dequeues one of them and returns it. If not, a new condition variable is allocated and returned.
 * 
 * Parameters:
 *    None.
 * 
 * Returns:
 *    ret - an empty condition variable, dequeued from the pool or newly allocated.
 */
condvar_t *empty_condvar() {
    condvar_t *ret;
    pthread_mutex_lock(&condvar_pool_mutex);
    ret = condvar_pool.len > 0 ? dequeue(&condvar_pool) : alloc_condvar();
    pthread_mutex_unlock(&condvar_pool_mutex);
    return ret;
}

/*
 * Function: release_condvar
 * -------------------------
 * Releases a condition variable by either enqueueing it back into the pool (condvar_pool) if there is space,
 * or freeing its memory if the pool is full.
 * 
 * Parameters:
 *    cv - a double pointer to the condition variable that is to be released.
 * 
 * Returns:
 *    Nothing.
 */
void release_condvar(condvar_t **cv) {
    atomic_init(&((*cv)->ref), 0);
    atomic_init(&((*cv)->cd), CV_NULL_CHANNEL_DESCRIPTOR);
    pthread_mutex_lock(&condvar_pool_mutex);
    if (condvar_pool.len == condvar_pool_max) {
        free_condvar(cv);
    } else {
        enqueue(&condvar_pool, *cv);
        *cv = NULL;
    }
    pthread_mutex_unlock(&condvar_pool_mutex);
}

/*
 * Function: init_condvar_pool
 * ---------------------------
 * Initializes the condition variable pool and its mutex. The maximum size of the pool is set based on the argument.
 *
 * Parameters:
 *    pool_max - The maximum size for the pool of condition variables.
 *
 * Returns:
 *    Returns 0 if the mutex was successfully initialized, and an error value otherwise.
 */
int init_condvar_pool(int pool_max) {
    condvar_pool_max = pool_max;
    condvar_pool.head = NULL;
    condvar_pool.tail = NULL;
    condvar_pool.len = 0;
    return pthread_mutex_init(&condvar_pool_mutex, NULL);
}