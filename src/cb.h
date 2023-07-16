#ifndef _LC_CB_H
#define _LC_CB_H

#include <stdio.h>
#include "libchannel.h"

// The `cbuff_t` structure defines a circular buffer that can store `rawdata_t` type data.
typedef struct {
    any_t *buff;
    int start;
    int end;
    size_t len;
    size_t cap;
} cbuff_t;

// `cb_init` function initializes a new circular buffer of a given size.
// Returns a pointer to the created buffer on success, NULL on failure.
extern cbuff_t *cb_init(size_t size);

// `cb_deinit` function deallocates the circular buffer pointed by its argument.
// After this function, the pointer is set to NULL.
extern void cb_free(cbuff_t **cb);

// `cb_write` function writes data to the circular buffer.
// Returns 0 on success, -1 if the buffer is full.
extern int cb_write(cbuff_t *cb, any_t data);

// `cb_read` function reads data from the circular buffer.
// Returns 0 on success, -1 if the buffer is empty.
extern int cb_read(cbuff_t *cb, any_t *data);

#endif