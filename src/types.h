/*
 * File: types.h
 * ----------------------------
 * This header file includes definitions for the 'any_t' structure and 
 * different variable type identifiers used in the library.
 *
 * The 'any_t' structure is a generic type that allows storing values of different types.
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
#ifndef _TYPES_H
#define _TYPES_H 1

#define VAR_INT8         0x00
#define VAR_INT16        0x01
#define VAR_INT32        0x02
#define VAR_INT64        0x03
#define VAR_FLOAT        0x04
#define VAR_DOUBLE       0x05
#define VAR_POINTER      0x06

#include <stdint.h>

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

#endif

