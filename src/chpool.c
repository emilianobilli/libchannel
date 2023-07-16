

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
static int next_channel = 1;

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
    channel_table[cd] = new_chan(len);
    pthread_mutex_unlock(&channel_table_mutex);
    return cd;
}

/*
 * Function: close_chan
 * --------------------
 * This function attempts to close a given channel. 
 *
 * The function first acquires a lock on the channel table, then checks if the
 * channel exists and if it is closeable according to the is_closeable function.
 * If the channel is closeable, it is removed from the channel table, deleted,
 * and the function returns 0 to indicate a successful operation. 
 *
 * If the channel does not exist, is not closeable, or any other errors occur, 
 * the function returns -1 to signal an unsuccessful operation. 
 *
 * The function handles unlocking the acquired locks before it returns, 
 * regardless of the operation's success or failure.
 *
 * Parameters:
 * cd: The channel descriptor of the channel to close.
 *
 * Returns:
 * 0 if the operation was successful, and -1 otherwise.
 */
int close_chan(int cd) {
    int ret;
    chan_t *chan;
    pthread_mutex_lock(&channel_table_mutex);
    chan = channel_table[cd];
    if (chan) {
        pthread_mutex_lock(&(chan->mutex));
        if (is_closeable(chan)) {
            channel_table[cd] = NULL;
            del_chan(chan);
            ret = 0;
        } else {
            ret = -1;
        }
        pthread_mutex_unlock(&(chan->mutex));
    } else {
        ret = -1;
    }
    pthread_mutex_unlock(&channel_table_mutex);
    return ret;
}

/*
 * Function: get_channel_from_table
 * -------------------------------
 * This function returns a pointer to the channel corresponding to the 'cd' 
 * identifier in the channel table.
 */
chan_t *get_channel_from_table(int cd) {
    chan_t *chan;
    pthread_mutex_lock(&channel_table_mutex);
    chan = channel_table[cd];
    pthread_mutex_unlock(&channel_table_mutex);
    return chan;
}

int init_channel_pool(void) {
    return pthread_mutex_init(&channel_table_mutex, NULL);
}