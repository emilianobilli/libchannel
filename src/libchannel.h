#ifndef _LIBCHANNEL_H
#define _LIBCHANNEL_H 1

#define VAR_INT8         0x00
#define VAR_INT16        0x01
#define VAR_INT32        0x02
#define VAR_INT64        0x03
#define VAR_FLOAT        0x04
#define VAR_DOUBLE       0x05
#define VAR_POINTER      0x06

#include <stdint.h>
#include <stdlib.h>

/* The 'any_t' structure is a generic type that allows storing values of different types.
 * It consists of an 'int type' which indicates the type of the value stored and a union
 * which contains different possible types for the value. It enables users to store
 * different types of variables (e.g., int32, int64, float, double, pointer, etc.) in a 
 * unified way.
 *
 * Structures:
 * any_t: A structure for generic types.
 *
 *      int type: An identifier indicating the type of value stored.
 *      union value: A union that contains different possible types for the value.
 *
 * Macros:
 * VAR_INT8 to VAR_POINTER: These macros are defined as different identifiers for the type 
 * of the value stored in 'any_t'.
 *
 * Note:
 * types.h should only be included once, hence the use of '_TYPES_H' definition to prevent 
 * multiple inclusions.
 */
typedef struct {
    int type;
    union {
        int32_t int32_val;
        int64_t int64_val;
        int16_t int16_val;
        int8_t  int8_val;
        float   float_val;
        double  double_val;
        void    *pointer_val;
    } value;
} any_t;

/*
 * Constants for Operation Types
 */
#define OP_SEND 0 // Constant for Send operation type
#define OP_RECV 1 // Constant for Receive operation type


#define SELECT_BLOCK    1
#define SELECT_NONBLOCK 0
#define OP_BLOCK        1
#define OP_NONBLOCK     0
/*
 * Structure: select_set_t
 * -----------------------
 * This structure represents a set of send or receive operations
 * on multiple channels. Each operation is defined by the channel 
 * descriptor, the operation type, the data to be sent (for send operations), 
 * and the address where the received data should be stored (for receive operations).
 * 
 * Members:
 *    cd:     The channel descriptor.
 *    op_type: The operation type. This should be either OP_SEND or OP_RECV.
 *    send:   A pointer to the data to be sent. This should be NULL for receive operations.
 *    recv:   A pointer to a location where the received data should be stored. This should be NULL for send operations.
 */
typedef struct {
    int cd;
    int op_type;
    any_t *send;
    any_t *recv;
} select_set_t;


extern int init_libchannel(void);


/*
 * Function: send_chan
 * -------------------
 * This function sends data to a channel.
 *
 * Parameters:
 *    cd   - the descriptor of the channel to send to.
 *    send - a pointer to the data to send.
 *
 * Returns:
 *    On success, it returns the descriptor of the channel where the data was sent.
 *    If the operation was not successful, it returns -1.
 */
extern int send_chan(int cd, any_t *send);
extern int send_chan_bctrl(int cd, any_t *send, int should_block);

/*
 * Function: recv_chan
 * -------------------
 * This function receives data from a channel.
 *
 * Parameters:
 *    cd   - the descriptor of the channel to receive from.
 *    recv - a pointer to a location where the received data should be stored.
 *
 * Returns:
 *    On success, it returns the descriptor of the channel from which the data was received.
 *    If the operation was not successful, it returns -1.
 */
extern int recv_chan(int cd, any_t *recv);
extern int recv_chan_bctrl(int cd, any_t *recv, int should_block);
/*
 * Function: select_chan_op
 * ------------------------
 * This function attempts to synchronize and communicate between threads
 * using a select operation on multiple channels.
 *
 * Parameters:
 * - set: a pointer to an array of `select_set_t` structures, representing the channels to be selected.
 * - n: the number of channels in the array.
 * - should_block: a int to check if the select needs to wait until one channel is ready or not
 *
 * Returns:
 * - Upon successful completion, the function returns 1.
 * - If there are no channels in the array (n == 0), the function returns -1.
 * - If the select operation is unsuccessful, the function goes into a blocking state, waiting for a condition to be signaled.
 * - The function returns the result of `select_chan` function, if the condition variable is signaled.
 */
extern int select_chan(select_set_t *set, size_t n, int should_block);

/*
 * Function: make_chan
 * ---------------------
 * This function creates a new channel and adds it to the channel table. 
 * It locks the channel table mutex to prevent conflicts, then initializes 
 * the new channel with a buffer of size 'len', empty queues for 
 * message sending and receiving, and a new mutex.
 * Returns the identifier of the created channel.
 */
extern int make_chan(size_t len);


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
extern int close_chan(int cd);

/*
 * Function: cap
 * --------------------
 * This function returns the capacity of the channel.
 *
 * The capacity of a channel is the maximum number of items 
 * that it can hold at any given time. This function will
 * return the capacity if the channel exists and has been 
 * properly initialized.
 *
 * If the channel is NULL or hasn't been properly initialized,
 * the function returns a zero.
 *
 * Parameters:
 * chan: The channel whose capacity we want to find out.
 *
 * Returns:
 * The capacity of the channel, or zero if the channel is not properly initialized.
 */
extern int cap(int cd);
/*
 * Function: len
 * --------------------
 * This function returns the current length of the channel.
 *
 * The length of a channel is the current number of items 
 * that it holds. This function will return the length if 
 * the channel exists and has been properly initialized.
 *
 * If the channel is NULL or hasn't been properly initialized,
 * the function returns a zero.
 *
 * Parameters:
 * chan: The channel whose length we want to find out.
 *
 * Returns:
 * The length of the channel, or zero if the channel is not properly initialized.
 */
extern int len(int cd);
#endif