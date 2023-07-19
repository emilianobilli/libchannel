#include <stdlib.h>
#include "chan.h"
#include "cb.h"

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
chan_t *new_chan(size_t len) {
    chan_t *chan = calloc(1, sizeof(chan_t));
    if (chan) {
        chan->cb = cb_init(len);
        chan->send_shift = NULL;
        chan->recv_shift = NULL;
        chan->sendq.len = 0;
        chan->sendq.head = NULL;
        chan->sendq.tail = NULL;
        chan->recvq.len = 0;
        chan->recvq.head = NULL;
        chan->recvq.tail = NULL;
        pthread_mutex_init(&(chan->mutex), NULL);
    }
    return chan;
}


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
void del_chan(chan_t *chan) {
    if (chan) {
        cb_free(&(chan->cb));
        pthread_mutex_destroy(&(chan->mutex));
        free(chan);
    }
}

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
int is_closeable(chan_t *chan) {
    return (chan->recv_shift == NULL && chan->recvq.len == 0 && chan->send_shift == NULL && chan->sendq.len == 0);
}


