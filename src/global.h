#ifndef _GLOBAL_H
#define _GLOBAL_H 1

#include "waitq.h"
#include "chan.h"

extern int init_condvar_pool(int pool_max);

extern void release_condvar(condvar_t **cv);

extern condvar_t *empty_condvar();

extern chan_t *get_channel_from_table(int);

extern int make_chan(size_t len);

extern int init_channel_pool();

#endif