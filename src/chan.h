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


/*    int pref;
    int retv;
    condvar_t *cond_var;
    while (queue->len > 0) {
        cond_var = queue->head->pcondvar;
        retv = pthread_mutex_lock(&(cond_var->mutex));
        assert(retv == 0);
        // Safe area, now I can check if the condition is expired
        if (cond_var->expired) {
            cond_var = dequeue(queue);
            pref = --(cond_var->pref);
            retv = pthread_mutex_unlock(&(cond_var->mutex));
            assert(retv == 0);
            if (pref == 0)
                release_condvar(&cond_var);
        } else {
            if (cond_var->cd == -1) {
                cond_var->cd = chan_desc;
                retv = pthread_cond_signal(&(cond_var->cond_var));
                assert(retv == 0);
                retv = pthread_mutex_unlock(&(cond_var->mutex));
                assert(retv == 0);
                break;
            } else {
                retv = pthread_mutex_unlock(&(cond_var->mutex));
                assert(retv == 0);
            }
        }
    }
}
/*

ChannelOperation operations[] = {
    { &channel1, &data1, 1 },  // Operación de recepción en el canal 1
    { &channel2, &data2, 0 },  // Operación de envío en el canal 2
    // más operaciones...
};
switch(select(operations, timeout)) {
    case channel1:

    case channel2:
}

*/
#endif