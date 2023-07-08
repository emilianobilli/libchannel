#ifndef _VAR_TYPES_H
#define _VAR_TYPES_H 1

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
        int64_t int64_t_val;
        int16_t int16_t_val;
        int8_t  int8_t_val;
        float   float_val;
        double  double_val;
        void    *pointer_val;
    } value;
} any_t;

#define PRIVATE static

#endif

