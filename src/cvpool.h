#ifndef _CVPOOL_H
#define _CVPOOL_H 1

#include "waitq.h"

extern int init_condvar_pool(int pool_max);

extern void release_condvar(condvar_t **cv);

extern condvar_t *empty_condvar();
#endif 