/*
package main

import (
	"fmt"
	"time"
)

func AfterDuration(d time.Duration) <- chan struct{} {
	c := make(chan struct{}, 1)
	go func() {
		time.Sleep(d)
		c <- struct{}{}
	}()
	return c
}

func main() {
	fmt.Println("Hi!")
	<- AfterDuration(time.Second)
	fmt.Println("Hello!")
	<- AfterDuration(time.Second)
	fmt.Println("Bye!")
}
*/

#include <libchannel.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

struct fnarg {
    int c;
    int d;
};

#define GO(func, arg) do { pthread_t __routine; pthread_create(&__routine, NULL, &func, arg); pthread_detach(__routine); } while(0)

void *function(void *arg) {
    struct fnarg *a = (struct fnarg *)arg;
    any_t nil;
    sleep(a->d);
    send_chan(a->c, &nil);
    free(arg);
    return NULL;
}

int after_duration(int d) {
    struct fnarg *arg = malloc(sizeof(struct fnarg));
    int c = make_chan(1);
    arg->c = c;
    arg->d = d;
    GO(function, arg);
    return c;
}


int main(void) {
    any_t nil;
    init_libchannel();

    printf("Hi\n");
    recv_chan(after_duration(1), &nil);
    printf("Hello!\n");
    recv_chan(after_duration(1), &nil);
    printf("Bye!\n");
}