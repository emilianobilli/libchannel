

#define MAX_CHANNELS 100

#include <pthread.h>
#include <stdlib.h>
#include "chan.h"
#include "waitq.h"

/*
 * Array: channel_table
 * --------------------
 * This is the channel table. Each entry is a pointer to a channel. 
 * It is initialized with all pointers as NULL.
 */
static chan_t *channel_table[MAX_CHANNELS] = { 0 };


/*
 * Variable: next_channel
 * ----------------------
 * This is a counter that indicates the next available channel in the channel table.
 */
static int next_channel = 0;

/*
 * Mutex: channel_table_mutex
 * --------------------------
 * This is a mutex used to prevent conflicts when accessing the channel table 
 * from different threads of execution.
 */
static pthread_mutex_t channel_table_mutex;

/*
 * Function: make_chan
 * ---------------------
 * This function creates a new channel and adds it to the channel table. 
 * It locks the channel table mutex to prevent conflicts, then initializes 
 * the new channel with a buffer of size 'len', empty queues for 
 * message sending and receiving, and a new mutex.
 * Returns the identifier of the created channel.
 */
int make_chan(size_t len) {
    int cd;
    pthread_mutex_lock(&channel_table_mutex);
    cd = next_channel++;
    channel_table[cd] = calloc(1, sizeof(chan_t));
    channel_table[cd]->cb = cb_init(len);
    channel_table[cd]->send_shift = NULL;
    channel_table[cd]->recv_shift = NULL;
    channel_table[cd]->sendq.len = 0;
    channel_table[cd]->sendq.head = NULL;
    channel_table[cd]->sendq.tail = NULL;
    channel_table[cd]->recvq.len = 0;
    channel_table[cd]->recvq.head = NULL;
    channel_table[cd]->recvq.tail = NULL;

    pthread_mutex_init(&(channel_table[cd]->mutex), NULL);
    pthread_mutex_unlock(&channel_table_mutex);
    return cd;
}

/*
 * Function: get_channel_from_table
 * -------------------------------
 * This function returns a pointer to the channel corresponding to the 'cd' 
 * identifier in the channel table.
 */
chan_t *get_channel_from_table(int cd) {
    return channel_table[cd];
}

int init_channel_pool(void) {
    return pthread_mutex_init(&channel_table_mutex, NULL);
}