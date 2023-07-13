#include <stdlib.h>
#include "cb.h"


// `cb_init` function initializes a new circular buffer of a given size.
// Returns a pointer to the created buffer on success, NULL on failure.
cbuff_t *cb_init(size_t size) {
    cbuff_t *ptr = calloc(1, sizeof(cbuff_t));
    if (ptr) {
        ptr->buff = calloc(size, sizeof(any_t));
        if (ptr->buff) {
            ptr->cap = size;
            return ptr;
        }
        free(ptr);
    }
    return NULL;
}

// `cb_deinit` function deallocates the circular buffer pointed by its argument.
// After this function, the pointer is set to NULL.
void cb_free(cbuff_t **cb) {
    if (cb && *cb) {
        if ((*cb)->buff) {
            free((*cb)->buff);
            (*cb)->buff = NULL;
        }
        free(*cb);
        cb = NULL;
    }
}

// `cb_write` function writes data to the circular buffer.
// Returns 1 on success, 0 if the buffer is full or cb in NULL.
int cb_write(cbuff_t *cb, any_t data) {
    if (!cb)
        return 0;
    if (cb->len == cb->cap) {
        return 0;  // El buffer está lleno, no se puede escribir
    }

    cb->buff[cb->end] = data;
    cb->end = (cb->end + 1) % cb->cap;
    cb->len++;

    return 1;
}

// `cb_read` function reads data from the circular buffer.
// Returns 1 on success, 0 if the buffer is empty or cb is NULL
int cb_read(cbuff_t *cb, any_t *data) {
    if (!cb)
        return 0;
    if (cb->len == 0) {
        return 0;  // El buffer está vacío, no se puede leer
    }

    *data = cb->buff[cb->start];
    cb->start = (cb->start + 1) % cb->cap;
    cb->len--;

    return 1;
}