# Definir el compilador
CC = gcc

CFLAGS = -fPIC -Wall -g
LDFLAGS = -lpthread

# Definir la biblioteca compartida
LIBRARY_SHARED = libchannel.so
LIBRARY_STATIC = libchannel.a

# Define los archivos fuente
SOURCES = atomic.c cb.c chan.c chpool.c cvpool.c init.c lock.c select.c waitq.c

OBJECTS = $(SOURCES:.c=.o)

all: $(LIBRARY_SHARED) $(LIBRARY_STATIC)

$(LIBRARY_SHARED): $(OBJECTS)
	$(CC) -shared -o $(LIBRARY_SHARED) $(OBJECTS) $(LDFLAGS)

$(LIBRARY_STATIC): $(OBJECTS)
	ar rcs $(LIBRARY_STATIC) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIBRARY_SHARED) $(LIBRARY_STATIC) $(OBJECTS)

install:
	cp $(LIBRARY_SHARED) /usr/local/lib/
	cp $(LIBRARY_STATIC) /usr/local/lib/
	cp libchannel.h /usr/local/include/libchannel.h
