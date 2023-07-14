/*
 * File: custom_atomic.h
 * ----------------------------
 * This header file includes definitions for atomic operations.
 *
 * The file provides two macros for incrementing and decrementing
 * atomic integer values, as well as two function prototypes for 
 * atomic subtraction and addition operations.
 * 
 * Macros:
 * ATOMIC_INC(a): Increment the atomic integer 'a' by 1.
 * ATOMIC_DEC(a): Decrement the atomic integer 'a' by 1.
 *
 * Functions:
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

#ifndef _CUSTOM_ATOMIC_H
#define _CUSTOM_ATOMIC_H


#include <stdatomic.h>

#define ATOMIC_INC(a) atomic_add(a,1)
#define ATOMIC_DEC(a) atomic_sub(a,1)

extern int atomic_sub(atomic_int *obj, int arg);

extern int atomic_add(atomic_int *obj, int arg);

#endif