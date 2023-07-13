# libchannel
Golang style channels in C

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
