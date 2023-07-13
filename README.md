# libchannel
Golang style channels in C

## The any_t Structure

The any_t structure is a way to store various types of data in a type-safe way. This is done through the use of a union, which can store different types of data in the same memory space. The actual type of the data is tracked using an int field named type.

Here is the any_t structure definition:

```
#define VAR_INT8         0x00
#define VAR_INT16        0x01
#define VAR_INT32        0x02
#define VAR_INT64        0x03
#define VAR_FLOAT        0x04
#define VAR_DOUBLE       0x05
#define VAR_POINTER      0x06

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
```

### The any_t structure has two main components:

1. type: An integer that represents the type of the data stored in the value field. It can have one of the following values:

- VAR_INT8: Indicates that value holds an 8-bit integer.
- VAR_INT16: Indicates that value holds a 16-bit integer.
- VAR_INT32: Indicates that value holds a 32-bit integer.
- VAR_INT64: Indicates that value holds a 64-bit integer.
- VAR_FLOAT: Indicates that value holds a single-precision floating-point number.
- VAR_DOUBLE: Indicates that value holds a double-precision floating-point number.
- VAR_POINTER: Indicates that value holds a pointer.

2. value: A union that can hold different types of data. The actual type of data it holds is specified by the type field.

Here's an example of how to use any_t to store a 32-bit integer:

```
any_t my_var;
my_var.type = VAR_INT32;
my_var.value.int32_val = 12345;
```

And here's an example of how to use any_t to store a pointer:

```
any_t my_var;
my_var.type = VAR_POINTER;
my_var.value.pointer_val = &some_variable;
```

When retrieving the data from an `any_t` variable, make sure to use the correct field in the `value` union, as indicated by the type field:

```
if (my_var.type == VAR_INT32) {
    int32_t my_int = my_var.value.int32_val;
    // Use my_int
} else if (my_var.type == VAR_POINTER) {
    void *my_pointer = my_var.value.pointer_val;
    // Use my_pointer
}
```


## Using the select_chan Function

The select_chan function is used to concurrently select and perform operations on multiple channels. This function takes three arguments:

```
int select_chan(select_set_t *set, size_t n, int should_block);
```

1. An array of select_set_t structures, where each structure represents an operation (send or receive) on a specific channel.
2. The size of the array (i.e., the number of operations to perform).
3. A third argument that indicates whether the function should block if none of the operations can be immediately performed.

### Here is an example of how to use select_chan:

```
for (;;) {
    int n;
    select_set_t op[] = {
        {a, OP_RECV, NULL, &v},  
        {b, OP_RECV, NULL, &v}
    };
    n = select_chan(op, 2, SELECT_BLOCK);
    switch(n) {
        case a:
             // Received from a.
        case b:
             // Received from b.
    }
}
```

In this example, select_chan will attempt to receive data from channels a and b. The function will block if there is no data available for receiving on any of the channels. When data is received on one of the channels, the function will store the data in the variable v.

### The select_set_t Structure

```
typedef struct {
    int cd;
    int op_type;
    any_t *send;
    any_t *recv;
} select_set_t;
```

The select_set_t structure is used to specify an operation on a specific channel. This structure has four fields:

- cd: The descriptor of the channel on which the operation will be performed.
- op_type: The type of operation to perform. Can be OP_SEND to send data or OP_RECV to receive data.
- send: A pointer to the data to send. This field is ignored if op_type is OP_RECV.
- recv: A pointer to a location to store received data. This field is ignored if op_type is OP_SEND.
  
In the above example, both a and b are channel descriptors, OP_RECV indicates that the operation to perform is a receive, and &v is a pointer to a location to store received data.
