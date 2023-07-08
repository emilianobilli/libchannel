#include "waitq.h"

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
    ret = cond_var.len > 0 ? dequeue(&condvar_pool) : alloc_condvar();
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
void release_condvar(cond_var **cv) {
    (*cv)->is_select = 0;
    (*cv)->cd = -1;
    pthread_mutex_lock(&condvar_pool_mutex);
    if (cond_var.len == condvar_pool_max) {
        free_condvar(cv);
    } else {
        enqueue(&condvar_pool, *cv);
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
    condvar.len = 0;
    return pthread_mutex_init(&condvar_pool_mutex, NULL);
}