#ifndef _LC_CHAN_
#define _LC_CHAN_ 1

#include <pthread.h>

#include "types.h"
#include "cb.h"
#include "waitq.h"

typedef struct {
    cbuff_t *cb;
    pthread_mutex_t  mutex;

    pthread_t *recv_shift;
    pthread_t *send_shift;
    waitq_t recvq;
    waitq_t sendq;
} chan_t;

#endif