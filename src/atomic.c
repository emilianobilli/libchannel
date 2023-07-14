
#include "atomic.h"

/*
 * Functions
 * ---------
 * atomic_sub: This function performs an atomic subtraction operation. 
 *             It takes an atomic integer object and an integer value 
 *             to subtract from the atomic integer. It returns the value 
 *             of the atomic integer after the subtraction operation.
 * 
 * atomic_add: This function performs an atomic addition operation. 
 *             It takes an atomic integer object and an integer value 
 *             to add to the atomic integer. It returns the value 
 *             of the atomic integer after the addition operation.
 */
int atomic_sub(atomic_int *obj, int arg) {
    int ret = atomic_fetch_sub(obj, arg);
    return ret - arg;
}

int atomic_add(atomic_int *obj, int arg) {
    int ret = atomic_fetch_add(obj, arg);
    return ret + arg;
}