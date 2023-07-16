/*
package main

import "log"
import "time"

type T = struct{}

func worker(id int, ready <-chan T, done chan<- T) {
	<-ready // block here and wait a notification
	log.Print("Worker#", id, " starts.")
	// Simulate a workload.
	time.Sleep(time.Second * time.Duration(id+1))
	log.Print("Worker#", id, " job done.")
	// Notify the main goroutine (N-to-1),
	done <- T{}
}

func main() {
	log.SetFlags(0)

	ready, done := make(chan T), make(chan T)
	go worker(0, ready, done)
	go worker(1, ready, done)
	go worker(2, ready, done)

	// Simulate an initialization phase.
	time.Sleep(time.Second * 3 / 2)
	// 1-to-N notifications.
	ready <- T{}; ready <- T{}; ready <- T{}
	// Being N-to-1 notified.
	<-done; <-done; <-done
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


struct arg {
    int id;
    int ready;
    int done;
};

void *worker(void *arg) {
    struct arg *a = arg;
    any_t nil;
    recv_chan(a->ready, &nil);
    printf("Worker #%d starts.\n", a->id);
    fflush(stdout);
    // Simulate a workload.
    sleep(2);
    printf("Worker #%d job done.\n", a->id);
    send_chan(a->done, &nil);
    return NULL;
}

int main(void) {
    int ready, done;
    struct arg a, b, c;
    any_t nil;
    
    init_libchannel();
    
    ready = make_chan(1);
    done  = make_chan(1);

    a.id = 1;
    a.ready = ready;
    a.done = done;

    b.id = 2;
    b.ready = ready;
    b.done = done;

    c.id = 3;
    c.ready = ready;
    c.done = done;

    GO(worker, &a);
    GO(worker, &b);
    GO(worker, &c);

    // Simulate an initialization phase.
    sleep(3);
	// 1-to-N notifications.
    send_chan(ready, &nil);    send_chan(ready, &nil);    send_chan(ready, &nil);
	// Being N-to-1 notified.
    recv_chan(done, &nil);    recv_chan(done, &nil);    recv_chan(done, &nil);
}