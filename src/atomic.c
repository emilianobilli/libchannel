
#include "atomic.h"

int atomic_sub(atomic_int *obj, int arg) {
    int ret = atomic_fetch_sub(obj, arg);
    return ret - arg;
}

int atomic_add(atomic_int *obj, int arg) {
    int ret = atomic_fetch_add(obj, arg);
    return ret + arg;
}