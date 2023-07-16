#include "chpool.h"
#include "cvpool.h"

int init_libchannel(void) {
    int ret;
    return (ret = init_channel_pool()) == 0 ? init_condvar_pool(10) : ret;
}