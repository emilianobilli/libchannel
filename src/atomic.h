#ifndef _CUSTOM_ATOMIC_H
#define _CUSTOM_ATOMIC_H


#include <stdatomic.h>

#define ATOMIC_INC(a) atomic_add(a,1)
#define ATOMIC_DEC(a) atomic_sub(a,1)

extern int atomic_sub(atomic_int *obj, int arg);

extern int atomic_add(atomic_int *obj, int arg);

#endif